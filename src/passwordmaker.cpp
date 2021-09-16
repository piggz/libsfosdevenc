#include "passwordmaker.h"

#include "passwordhwcrypt.h"
#include "passwordplain.h"

using namespace DevEnc;

PasswordMaker* PasswordMaker::s_instance = nullptr;

PasswordMaker::PasswordMaker(QObject *parent) : QObject(parent)
{
}

PasswordMaker* PasswordMaker::instance()
{
  if (!s_instance) s_instance = new PasswordMaker();
  return s_instance;
}

QStringList PasswordMaker::types() const
{
  QStringList t;
  if (PasswordHWCrypt::available()) t << PasswordHWCrypt::type();
  if (PasswordPlain::available()) t << PasswordPlain::type();
  return t;
}

QString PasswordMaker::description(QString type) const
{
  if (type == PasswordHWCrypt::type()) return PasswordHWCrypt::description();
  if (type == PasswordPlain::type()) return PasswordPlain::description();
  return "";
}

QString PasswordMaker::descriptionLong(QString type) const
{
  if (type == PasswordHWCrypt::type()) return PasswordHWCrypt::descriptionLong();
  if (type == PasswordPlain::type()) return PasswordPlain::descriptionLong();
  return "";
}

Password* PasswordMaker::newPassword(QString type)
{
  if (type == PasswordHWCrypt::type()) return new PasswordHWCrypt();
  if (type == PasswordPlain::type()) return new PasswordPlain();
  return nullptr;
}
