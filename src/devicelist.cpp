#include "devicelist.h"

#include <QSettings>

#include <QDebug>

using namespace DevEnc;

DeviceList* DeviceList::s_instance = nullptr;

DeviceList::DeviceList(QObject *parent) : QObject(parent)
{
  // Load available devices
  QSettings device_config(CONFIG_DIR "/devices.ini", QSettings::IniFormat);
  for (QString group: device_config.childGroups())
    {
      device_config.beginGroup(group);
      QSharedPointer<Device> p(new Device(device_config));
      m_devices.append(p);
      device_config.endGroup();
    }
}

DeviceList* DeviceList::instance()
{
  if (!s_instance) s_instance = new DeviceList();
  return s_instance;
}

bool DeviceList::initNeeded() const
{
  for (auto p: m_devices)
    if (!p->initialized())
      return true;
  return false;
}

QStringList DeviceList::devices() const
{
  QStringList l;
  for (auto p: m_devices)
    l.append(p->id());
  return l;
}

Device* DeviceList::device(const QString name)
{
  for (auto p: m_devices)
    if (p->id() == name)
      return p.data();
  return nullptr;
}

Device* DevEnc::DeviceList::nextDevice()
{
  int d = m_current_device + 1;
  if (d >= m_devices.length())
    return nullptr;
  m_current_device = d;
  return m_devices[m_current_device].data();
}
