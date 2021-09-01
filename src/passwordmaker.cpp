#include "passwordmaker.h"

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
  return { "plain" };
}

QString PasswordMaker::description(QString type) const
{
  if (type == "plain") return "";
  return "";
}

Password* PasswordMaker::make(QString type)
{
  return nullptr;
}
