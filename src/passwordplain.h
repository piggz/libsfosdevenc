#ifndef DEVENC_PASSWORDPLAIN_H
#define DEVENC_PASSWORDPLAIN_H

#include "password.h"

namespace DevEnc {

  class PasswordPlain : public DevEnc::Password
  {
  public:
    explicit PasswordPlain(QObject *parent = nullptr);

    virtual QByteArray get(QString /*mapper*/);

  public: // static
    static bool available();
    static QString type();
    static QString description();
    static QString descriptionLong();
  };

} // namespace DevEnc

#endif // DEVENC_PASSWORDPLAIN_H
