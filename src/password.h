#ifndef PASSWORD_H
#define PASSWORD_H

#include <QByteArray>
#include <QObject>
#include <QString>

namespace DevEnc {
  class Password : public QObject
  {
    Q_OBJECT

    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

  public:
    explicit Password(QObject *parent = nullptr);

    QString password() const { return m_password; }
    void setPassword(QString p);

    // override in implementations
    virtual QByteArray get() { return QByteArray(); }
    Q_INVOKABLE virtual QString type() { return QString(); }
    Q_INVOKABLE virtual QString description() { return QString(); }

  signals:
    void passwordChanged();

  protected:
    QString m_password;
  };
}

#endif // PASSWORD_H
