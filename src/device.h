#ifndef DEVENC_DEVICE_H
#define DEVENC_DEVICE_H

#include <QByteArray>
#include <QObject>
#include <QSettings>

namespace DevEnc {

  class Device : public QObject
  {
    Q_OBJECT

    Q_PROPERTY(bool deviceAvailable READ deviceAvailable)
    Q_PROPERTY(bool encrypted READ encrypted NOTIFY encryptedChanged)
    Q_PROPERTY(bool initialized READ initialized NOTIFY initializedChanged)
    Q_PROPERTY(QString id READ id)
    Q_PROPERTY(QString name READ name)

  public:
    Device(QObject *parent = nullptr);
    Device(QSettings &settings, QObject *parent = nullptr);

    Q_INVOKABLE QString getRecoveryPassword() const;
    bool removeRecoveryPasswordCopy();

    bool deviceAvailable() const;
    bool encrypted() const;
    QString id() const { return m_id; }
    bool initialized() const { return m_state != StateReset; }
    QString name() const { return m_name; }

    // set device either to be encrypted or not
    Q_INVOKABLE bool setEncryption(bool encrypt);

    // password
    Q_INVOKABLE bool addPasswordPlain(QByteArray password, QByteArray new_password);

    bool wantEncrypted() const { return m_state == StateEncrypted; }
    bool wantPlain() const { return m_state == StatePlain; }

  signals:
    void encryptedChanged();
    void initializedChanged();

  private:
    bool createFile();
    bool deleteFile();

    void createRecoveryPassword();
    bool writeRecoveryPasswordCopy();

    bool encryptAndFormat();
    bool format();

    bool createSystemDConfig(bool enc);

  private:
    enum State {
      StateReset,
      StateEncrypted,
      StatePlain
    };

    enum Type {
      TypeDevice,
      TypeFile
    };

  private:
    QString m_id;
    QString m_name;
    QString m_device;
    QString m_mapper;
    QString m_mount;
    Type m_type{TypeDevice};
    uint32_t m_size_mb;
    State m_state{StateReset};

    // internal variables
    QString m_recovery_password;
  };

} // namespace DevEnc

#endif // DEVENC_DEVICE_H
