#include "DeviceType.h"


std::ostream& operator<<(std::ostream& os, DeviceType deviteType) {
    switch (deviteType) {
        case CPU:
            os << "CPU";
            break;
        case GPU:
            os << "GPU";
            break;
        case NB:
            os << "NB";
            break;
        case RAM:
            os << "RAM";
            break;
        case VPU:
            os << "VPU";
            break;
        case AP:
            os << "AP";
            break;
        case SB:
            os << "SB";
            break;
        case MA:
            os << "MA";
            break;
        case DC:
            os << "DC";
            break;
        default:
            os << "UnknownDeviceType";
    }

    return os;
}
