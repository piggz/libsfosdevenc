#include "devicelist.h"

#include <iostream>
using namespace std;

int main()
{
  DevEnc::DeviceList &devlist = *DevEnc::DeviceList::instance();
  cout << "Init needed: " << devlist.initNeeded() << "\n";
  DevEnc::Device *device = devlist.device("home_in_file");
  cout << device->initialized() << "\n";
  cout << "Available: " << device->deviceAvailable() << "\n";
  cout << "Encrypted: " << device->encrypted() << "\n";

  bool ret = device->setEncryption(true);
  cout << "setEncrypted: " << ret << "\n";

//  cout << device->getRecoveryPassword().toStdString() << "\n";
//  cout << device->removeRecoveryPasswordCopy() << "\n";

  return 0;
}
