#include "devicelist.h"

#include <QSettings>

#include <QDebug>

using namespace DevEnc;

DeviceList::DeviceList(QObject *parent) : QObject(parent)
{
  // Load available devices
  QSettings device_config(CONFIG_DIR "/devices.ini", QSettings::IniFormat);
  for (QString group: device_config.childGroups())
    {
      device_config.beginGroup(group);
      QSharedPointer<Device> p(new Device(device_config));
      m_devices[group] = p;
      device_config.endGroup();
    }
}

bool DeviceList::initNeeded() const
{
  for (auto p: m_devices)
    if (!p->isInitialized())
      return true;
  return false;
}

Device* DeviceList::device(const QString name)
{
  if (m_devices.contains(name))
    return m_devices.value(name).data();
  return nullptr;
}
