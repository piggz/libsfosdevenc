#ifndef DEVENC_DEVICELIST_H
#define DEVENC_DEVICELIST_H

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <QString>

#include "device.h"

namespace DevEnc {

  class DeviceList : public QObject
  {
    Q_OBJECT
  public:
    explicit DeviceList(QObject *parent = nullptr);

    Q_INVOKABLE bool initNeeded() const;
    Q_INVOKABLE QStringList devices() const;
    Q_INVOKABLE DevEnc::Device* device(const QString name);

    Q_INVOKABLE DevEnc::Device* nextDevice();
    Q_INVOKABLE void resetNextDevice() { m_current_device = -1; }

  signals:

  public: // static
    static DeviceList* instance();

  private:
    QList< QSharedPointer<Device> > m_devices;
    int m_current_device{-1};

  private:
    static DeviceList *s_instance;

  };

} // namespace DevEnc

#endif // DEVENC_DEVICELIST_H
