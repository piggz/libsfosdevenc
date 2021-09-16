#include "passwordhwcrypt.h"

#include <QFileInfo>
#include <QProcess>
#include <QDebug>

#include <iostream>

using namespace DevEnc;

#define DELAY_BETWEEN_TRIES "3" // in seconds

PasswordHWCrypt::PasswordHWCrypt(QObject *parent) : DevEnc::Password(parent)
{
}

bool PasswordHWCrypt::available()
{
  return QFileInfo::exists(HWCRYPT_CMD);
}

QByteArray PasswordHWCrypt::get(QString mapper)
{
  // phase 1: generate hw key and salt if needed. if they are available already, it becomes noop
  {
    QProcess proc;
    proc.start(HWCRYPT_KEY_GEN_CMD,
               QStringList() << mapper << DELAY_BETWEEN_TRIES);

    // exit on error
    if (!proc.waitForFinished(-1) ||
        proc.exitStatus() != QProcess::NormalExit ||
        proc.exitCode() != 0)
      {
        std::cerr << "PasswordHWCrypt::get: Error 1: " << proc.readAllStandardError().data() << "\n";
        std::cerr << "PasswordHWCrypt::get: Output 1: " << proc.readAllStandardOutput().data() << "\n";
        std::cerr << "PasswordHWCrypt::get: Error while running hwcrypt-key-gen\n";
        return QByteArray();
      }
  }

  // phase 2: pipe password through hwcrypt
  QProcess proc;
  proc.start(HWCRYPT_KEY_CMD,
             QStringList() << mapper);

  QByteArray w = m_password.toLatin1();
  if (w.size() != proc.write(w))
    {
      std::cerr << "PasswordHWCrypt::get: Error while writing data to hwcrypt-key\n";
      return QByteArray();
    }
  proc.closeWriteChannel();

  // exit on error
  if (!proc.waitForFinished(-1) ||
      proc.exitStatus() != QProcess::NormalExit ||
      proc.exitCode() != 0)
    {
      std::cerr << "PasswordHWCrypt::get: Error 2: " << proc.readAllStandardError().data() << "\n";
      std::cerr << "PasswordHWCrypt::get: Output 2: " << proc.readAllStandardOutput().data() << "\n";
      std::cerr << "PasswordHWCrypt::get: Error while running hwcrypt-key\n";
      return QByteArray();
    }

  // read output
  QByteArray pwd = proc.readAllStandardOutput();
  QByteArray err = proc.readAllStandardError();

  if (err.size() > 0)
    std::cout << "PasswordHWCrypt::get StdErr: " << err.data() << "\n";

  return pwd;
}

QString PasswordHWCrypt::type()
{
  return QStringLiteral("hwcrypt");
}

QString PasswordHWCrypt::description()
{
  return tr("Hardware Keystore");
}

QString PasswordHWCrypt::descriptionLong()
{
  return tr("Password for opening encrypted file system with preprocessing using hardware keystore provided by Android drivers.");
}
