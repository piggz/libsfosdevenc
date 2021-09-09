#include "device.h"

#include <iostream>
#include <libcryptsetup.h>

using namespace DevEnc;

// helper macros
#define OPCHECK(op, msg, res) if (!(op)) { \
  std::cerr << "DevEnc::DevKey:" << __LINE__ << ": " << msg << ". Device: " << m_device.toStdString() << "\n"; \
  return res; }
#define OPCHECK_CRYPT(op, msg, res) if (!(op)) { \
  crypt_free(cd); \
  std::cerr << "DevEnc::DevKey:" << __LINE__ << ": " << msg << ". Device: " << m_device.toStdString() << "\n"; \
  return res; }

bool Device::addPassword(Password *passwordObj, Password *new_passwordObj)
{
  QByteArray password; if (passwordObj) password = passwordObj->get();
  QByteArray new_password; if (new_passwordObj) new_password = new_passwordObj->get();

  OPCHECK(m_state == StateEncrypted, "Cannot add password to non-encrypted device", false);
  OPCHECK(!new_password.isEmpty(), "Cannot add empty password", false);
  if (password.isEmpty()) password = m_recovery_password.toLatin1();
  OPCHECK(!password.isEmpty(), "Cannot add new password without providing another one", false);

  struct crypt_device *cd;

  OPCHECK(crypt_init(&cd, m_device.toLatin1().data()) == 0, "crypt_init() failed", false);
  OPCHECK_CRYPT(crypt_load(cd, CRYPT_LUKS, NULL) == 0, "Failed to load LUKS header", false);

  OPCHECK_CRYPT(crypt_keyslot_add_by_passphrase(cd,
                                                CRYPT_ANY_SLOT,
                                                password.data(),
                                                password.size(),
                                                new_password.data(),
                                                new_password.size()) >= 0,
                "Failed to add new password", false);

  crypt_free(cd);
  return true;
}


int Device::freePasswordSlots()
{
  if (m_state != StateEncrypted) return 0;

  struct crypt_device *cd;

  OPCHECK(crypt_init(&cd, m_device.toLatin1().data()) == 0, "crypt_init() failed", 0);
  OPCHECK_CRYPT(crypt_load(cd, CRYPT_LUKS, NULL) == 0, "Failed to load LUKS header", 0);

  int mx = crypt_keyslot_max(crypt_get_type(cd));
  OPCHECK_CRYPT(mx > 0, "Failed to determine number of keyslots", 0);

  int f = 0;
  for (int i=0; i < mx; ++i)
    {
      crypt_keyslot_info ki = crypt_keyslot_status(cd, i);
      if (ki == CRYPT_SLOT_INACTIVE) ++f;
    }

  crypt_free(cd);
  return f;
}

int Device::usedPasswordSlots()
{
  if (m_state != StateEncrypted) return 0;

  struct crypt_device *cd;

  OPCHECK(crypt_init(&cd, m_device.toLatin1().data()) == 0, "crypt_init() failed", 0);
  OPCHECK_CRYPT(crypt_load(cd, CRYPT_LUKS, NULL) == 0, "Failed to load LUKS header", 0);

  int mx = crypt_keyslot_max(crypt_get_type(cd));
  OPCHECK_CRYPT(mx > 0, "Failed to determine number of keyslots", 0);

  int f = 0;
  for (int i=0; i < mx; ++i)
    {
      crypt_keyslot_info ki = crypt_keyslot_status(cd, i);
      if (ki == CRYPT_SLOT_ACTIVE || ki == CRYPT_SLOT_ACTIVE_LAST) ++f;
    }

  crypt_free(cd);
  return f;
}
