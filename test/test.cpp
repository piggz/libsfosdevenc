#include "devicelist.h"

#include <iostream>
using namespace std;

int main()
{
  DevEnc::DeviceList devlist;
  cout << "Init needed: " << devlist.initNeeded() << "\n";
  DevEnc::Device *device = devlist.device("home_in_file");
  cout << device->isInitialized() << "\n";
  cout << "Available: " << device->isDeviceAvailable() << "\n";
  cout << "Encrypted: " << device->isEncrypted() << "\n";

//  bool ret = device->setEncryption(true);
//  cout << "Encrypted: " << ret << "\n";

  cout << device->getRecoveryPassword().toStdString() << "\n";
  cout << device->removeRecoveryPasswordCopy() << "\n";

  return 0;
}
