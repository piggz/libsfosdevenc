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

    bool initNeeded() const;
    QStringList devices() const { return m_devices.keys(); }
    Device* device(const QString name);

  signals:

  private:
    QMap< QString, QSharedPointer<Device> > m_devices;

  };

} // namespace DevEnc

#endif // DEVENC_DEVICELIST_H
