#ifndef DEVENC_DEVICE_H
#define DEVENC_DEVICE_H

#include <QObject>
#include <QSettings>

namespace DevEnc {

  class Device : public QObject
  {
    Q_OBJECT
  public:
    explicit Device(QSettings &settings, QObject *parent = nullptr);

    QString getRecoveryPassword() const;
    bool removeRecoveryPasswordCopy();

    bool isDeviceAvailable() const;
    bool isEncrypted() const;
    bool isInitialized() const { return m_state != StateReset; }

    // set device either to be encrypted or not
    bool setEncryption(bool encrypt);
    bool setPasswordPlain(QString password);

    bool wantEncrypted() const { return m_state == StateEncrypted; }
    bool wantPlain() const { return m_state == StatePlain; }

  signals:

  private:
    bool createFile();
    bool deleteFile();

    void createRecoveryPassword();
    bool writeRecoveryPasswordCopy();

    bool encryptAndFormat();

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
