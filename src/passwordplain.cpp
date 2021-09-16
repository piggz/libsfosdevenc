#include "passwordplain.h"

using namespace DevEnc;

PasswordPlain::PasswordPlain(QObject *parent) : DevEnc::Password(parent)
{
}

bool PasswordPlain::available()
{
  return true;
}

QByteArray PasswordPlain::get(QString /*mapper*/)
{
  return m_password.toLatin1();
}

QString PasswordPlain::type()
{
  return QStringLiteral("plain");
}

QString PasswordPlain::description()
{
  return tr("Plain text");
}

QString PasswordPlain::descriptionLong()
{
  return tr("Password for opening encrypted file system without addition preprocessing. "
            "This type of password is used by recovery. It is recommended to set "
            "long password for security.");
}
