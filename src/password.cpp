#include "password.h"

using namespace DevEnc;

Password::Password(QObject *parent) : QObject(parent)
{
}

void Password::setPassword(QString p)
{
  m_password = p;
  emit passwordChanged();
}
