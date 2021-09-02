#ifndef DEVENC_PASSWORDMAKER_H
#define DEVENC_PASSWORDMAKER_H

#include <QObject>
#include <QStringList>

#include "password.h"

namespace DevEnc {

  class PasswordMaker : public QObject
  {
    Q_OBJECT
  private:
    explicit PasswordMaker(QObject *parent = nullptr);

  public:
    Q_INVOKABLE QStringList types() const;
    Q_INVOKABLE QString description(QString type) const;
    Q_INVOKABLE QString descriptionLong(QString type) const;

    Password* make(QString type);

  public: // static
    static PasswordMaker* instance();

  private:
    static PasswordMaker* s_instance;

  };

} // namespace DevEnc

#endif // DEVENC_PASSWORDMAKER_H
