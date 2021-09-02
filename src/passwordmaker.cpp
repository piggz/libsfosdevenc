#include "passwordmaker.h"

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
  return { PasswordPlain::type() };
}

QString PasswordMaker::description(QString type) const
{
  if (type == PasswordPlain::type()) return PasswordPlain::description();
  return "";
}

QString PasswordMaker::descriptionLong(QString type) const
{
  if (type == PasswordPlain::type()) return PasswordPlain::descriptionLong();
  return "";
}

Password* PasswordMaker::newPassword(QString type)
{
  if (type == PasswordPlain::type()) return new PasswordPlain();
  return nullptr;
}
