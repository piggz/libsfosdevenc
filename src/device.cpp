#include "device.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QRandomGenerator>
#include <QTemporaryDir>

#include <exception>
#include <fstream>
#include <libcryptsetup.h>
#include <iostream>

using namespace DevEnc;

#define OPCHECK(op, msg) if (!(op)) { \
  std::cerr << "DevEnc::Device:" << __LINE__ << ": " << msg << ". Device: " << m_device.toStdString() << "\n"; \
  return false; }
#define OPCHECK_CRYPT(op, msg) if (!(op)) { \
  crypt_free(cd); \
  std::cerr << "DevEnc::Device:" << __LINE__ << ": " << msg << ". Device: " << m_device.toStdString() << "\n"; \
  return false; }

Device::Device(QSettings &settings, QObject *parent) : QObject(parent)
{
  m_id = settings.group();

  m_name = settings.value("name").toString();
  if (m_name == "Home") m_name = tr("Home");
  else if (m_name == "SD Card") m_name = tr("SD Card");

  m_device = settings.value("device").toString();
  m_mapper = settings.value("mapper").toString();
  m_mount = settings.value("mount").toString();

  QString t = settings.value("type").toString();
  if (t == "device") m_type = TypeDevice;
  else if (t == "file") m_type = TypeFile;
  else throw std::runtime_error(m_id.toStdString() + ": Missing type of device");

  m_size_mb = settings.value("size_mb", 0).toInt();
  if (m_size_mb <= 0 && m_type == TypeFile)
    throw std::runtime_error(m_id.toStdString() + ": Missing size of allocated file");

  QString s = settings.value("state", "Reset").toString();
  if (s == "Reset") m_state = StateReset;
  else if (s == "Encrypted") m_state = StateEncrypted;
  else if (s == "Plain") m_state = StatePlain;
  else
    throw std::runtime_error(m_id.toStdString() + ": Unknown state of the device");
}

bool Device::isDeviceAvailable() const
{
  QFileInfo fi(m_device);

  if (m_type==TypeDevice)
    return fi.exists();

  if (fi.exists() && fi.isFile())
    return true;
  else if (fi.exists())
    return false; // if it exists it should be file

  // file can be made if needed in directory
  return fi.dir().exists();
}

bool Device::isEncrypted() const
{
  OPCHECK(isDeviceAvailable(), "Device is not available");

  struct crypt_device *cd;

  OPCHECK(crypt_init(&cd, m_device.toLocal8Bit().data()) == 0, "crypt_init() failed");
  OPCHECK_CRYPT(crypt_load(cd, CRYPT_LUKS, NULL) == 0, "crypt_load() failed on device");

  qDebug() << crypt_get_type(cd) << crypt_get_cipher(cd) << crypt_get_cipher_mode(cd) << crypt_get_iv_offset(cd) << crypt_get_volume_key_size(cd);

  // cleanup
  crypt_free(cd);
  return true;
}

bool Device::setEncryption(bool enc)
{
  OPCHECK(!isInitialized(), "Settings encryption can be called on noninitialized device only");
  OPCHECK(isDeviceAvailable(), "Device is not available");

  // preparation for files
  if (m_type == TypeFile)
    {
      if (enc) { OPCHECK(createFile(), "Failed to create file"); }
      else { OPCHECK(deleteFile(), "Failed to delete file"); }
    }

  // encrypt and format the volume
  if (enc)
    {
      OPCHECK(encryptAndFormat(), "Failed to encrypt and format device");
    }

  // update systemd configuration
  OPCHECK(createSystemDConfig(enc), "Failed to setup SystemD configuration");

  // record changes in configuration
  QSettings settings(CONFIG_DIR "/devices.ini", QSettings::IniFormat);
  settings.beginGroup(m_id);
  settings.setValue("state", enc ? "Encrypted" : "Plain");
  m_state = (enc ? StateEncrypted : StatePlain);

  return true;
}

QString Device::getRecoveryPassword() const
{
  if (!m_recovery_password.isEmpty()) return m_recovery_password;

  // load from stored copy and show that. but never load it to
  // local var to distinguish between initialization state when
  // device was just formatted and the state when password was
  // requested by user for backing it up
  QDir dir(m_mount);

  std::ifstream fin( dir.absoluteFilePath(RECOVERY_PASSWORD_FILE).toLocal8Bit().data() );
  if (!fin) return QString();
  std::string pwd;
  fin >> pwd;
  return QString::fromStdString(pwd);
}

bool Device::removeRecoveryPasswordCopy()
{
  QDir dir(m_mount);
  OPCHECK(dir.remove(RECOVERY_PASSWORD_FILE), "Failed to remove recovery password copy");
  return true;
}


////////////////////////////////////////////
/// Private methods

bool Device::encryptAndFormat()
{
  // initialize encryption
  struct crypt_device *cd;

  OPCHECK(crypt_init(&cd, m_device.toLocal8Bit().data()) == 0, "crypt_init() failed");

  OPCHECK_CRYPT(crypt_format(cd,            /* crypt context */
                             CRYPT_LUKS2,   /* LUKS2 is a new LUKS format; use CRYPT_LUKS1 for LUKS1 */
                             "aes",         /* used cipher */
                             "xts-plain64", /* used block mode and IV */
                             NULL,          /* generate UUID */
                             NULL,          /* generate volume key from RNG */
                             512 / 8,       /* 512bit key - here AES-256 in XTS mode, size is in bytes */
                             NULL           /* default parameters */) == 0,
                "Failed to crypt_format()");

  createRecoveryPassword();
  OPCHECK_CRYPT(crypt_keyslot_add_by_volume_key(cd, /* crypt context */
                                                CRYPT_ANY_SLOT,     /* just use first free slot */
                                                NULL,               /* use internal volume key */
                                                0,                  /* unused (size of volume key) */
                                                m_recovery_password.toLocal8Bit().data(),
                                                m_recovery_password.length()) >= 0,
                "Failed to add recovery password");

  // open encrypted volume
  OPCHECK_CRYPT(crypt_activate_by_passphrase(cd,
                                             m_mapper.toLocal8Bit().data(),
                                             CRYPT_ANY_SLOT,
                                             m_recovery_password.toLocal8Bit().data(),
                                             m_recovery_password.length(),
                                             0) >= 0,
                "Failed to activate device");

  // format
  OPCHECK_CRYPT(QProcess::execute("mkfs.ext4",
                                  QStringList() << "/dev/mapper/" + m_mapper) == 0,
                "Failed to format filesystem");

  // mount and store recovery password
  OPCHECK_CRYPT(writeRecoveryPasswordCopy(), "Failed to store recovery password");

  // close encrypted volume
  OPCHECK_CRYPT(crypt_deactivate(cd, m_mapper.toLocal8Bit().data()) == 0,
                "Failed to deactivate device");

  crypt_free(cd);
  return true;
}


// recovery password
void Device::createRecoveryPassword()
{
  const QString chars("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  const int nc = chars.length();

  m_recovery_password = QString();
  for (int slot=0; slot < 8; ++slot)
    {
      if (!m_recovery_password.isEmpty())
        m_recovery_password += "-";
      for (int i=0; i < 5; ++i)
        m_recovery_password += chars[QRandomGenerator::global()->bounded(nc)];
    }

  // TODO: DELETE THIS!!!!
  qDebug() << "Recovery: " << m_recovery_password;
}

bool Device::writeRecoveryPasswordCopy()
{
  // mount filesystem
  QTemporaryDir tmpDir;
  OPCHECK(tmpDir.isValid(), "Failed to create temporary directory");
  OPCHECK(QProcess::execute("mount",
                            QStringList()
                            << "/dev/mapper/" + m_mapper
                            << tmpDir.path().toLocal8Bit().data()
                            ) == 0,
          "Failed to mount filesystem for temporary access");

  // write password
  QFileInfo fi(RECOVERY_PASSWORD_FILE);
  QString dpath = fi.dir().path();
  QDir dir(tmpDir.path());
  OPCHECK(dir.mkpath(dpath), "Failed to make folder for storing recovery password");

  QString abspath = dir.absoluteFilePath(fi.filePath());
  std::ofstream fout(abspath.toLocal8Bit().data());
  OPCHECK(fout, "Failed to open file for writing recovery password");
  fout << m_recovery_password.toStdString() << "\n";
  OPCHECK(fout, "Failed to write recovery password");
  fout.close();

  // set permissions
  OPCHECK(QFile::setPermissions(abspath, QFileDevice::ReadOwner | QFileDevice::WriteOwner),
          "Failed to set recovery password file permissions");
  OPCHECK(QFile::setPermissions(dir.absoluteFilePath(dpath),
                                QFileDevice::ReadOwner | QFileDevice::WriteOwner),
          "Failed to set permissions for local configuration directory");

  // unmount
  OPCHECK(QProcess::execute("umount",
                            QStringList()
                            << "/dev/mapper/" + m_mapper
                            ) == 0,
          "Failed to unmount filesystem after storing recovery password");

  return true;
}

// File operations
bool Device::createFile()
{
  QFileInfo fi(m_device);

  if (fi.exists())
    {
      QDir dir;
      OPCHECK(dir.remove(fi.absoluteFilePath()), "Failed to remove file");
    }

  std::ofstream fout(fi.absoluteFilePath().toLocal8Bit().data(), std::ios::binary | std::ios::out);
  OPCHECK(fout,"Failed to open file");
  fout.seekp( ((uint64_t)m_size_mb) * 1024*1024 );
  OPCHECK(fout,"Failed to seek in file initialization");
  fout.write("", 1);
  OPCHECK(fout,"Failed to write file");

  return true;
}

bool Device::deleteFile()
{
  QFileInfo fi(m_device);

  if (fi.exists())
    {
      QDir dir;
      OPCHECK(dir.remove(fi.absoluteFilePath()), "Failed to remove file");
    }

  return true;
}

// SystemD units
bool Device::createSystemDConfig(bool enc)
{
  return true;
}
