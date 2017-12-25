#ifndef DEVICETYPE_H
#define DEVICETYPE_H

#include <iostream>


enum DeviceType {
    CPU,
    GPU, NB,// RAM,
    // VPU, AP, SB, MA, DC,
    AmountOfDeviceTypes
};


std::ostream& operator<<(std::ostream& os, DeviceType deviteType);

#endif
