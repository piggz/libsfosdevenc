#ifndef DEVENC_PASSWORDHWCRYPT_H
#define DEVENC_PASSWORDHWCRYPT_H

#include "password.h"

namespace DevEnc {

  class PasswordHWCrypt : public DevEnc::Password
  {
  public:
    explicit PasswordHWCrypt(QObject *parent = nullptr);

    virtual QByteArray get(QString mapper);

  public: // static
    static bool available();
    static QString type();
    static QString description();
    static QString descriptionLong();
  };

} // namespace DevEnc

#endif // DEVENC_PASSWORDHWCRYPT_H
