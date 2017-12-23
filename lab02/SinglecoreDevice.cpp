#include "SinglecoreDevice.h"


SinglecoreDevice::SinglecoreDevice(
        // DeviceType type,
        double tau,
        std::map<DeviceType, double> transitionMap) :
        Device(tau, transitionMap),
        m_FinishTime(INF),
        m_TotalWorkTime(0.0) {}


void SinglecoreDevice::startIfCan(double currentGlobalTime) {
    if (m_AmountOfAwaitingTasks == 0) {
        // No awaiting tasks => nothing to start
        // m_FinishTime is already set to INF in finishTask()
        return;
    }

    if (m_FinishTime < INF) {
        // Busy => can't start anything
        // m_FinishTime is already set to some non-infinite value
        return;
    }

    // There is a task available and we are free => start
    // m_FinishTime == INF
    const double workTime = generateWorkTime();
    m_TotalWorkTime += workTime;
    m_FinishTime = currentGlobalTime + workTime;
    m_AmountOfAwaitingTasks--;
}


double SinglecoreDevice::generateWorkTime() const {
    return generatePoissonTime(m_Tau);
}


double SinglecoreDevice::getFinishTime() const {
    return m_FinishTime;
}


DeviceType SinglecoreDevice::finishTask() {
    m_FinishTime = INF;

    return pickDestinationDeviceType();
}


double SinglecoreDevice::getLoadCoeff(double totalTime) const {
    const double realWorkTime = (m_FinishTime < INF) ?
        // subtract the time that didn't get the chance to work
        (m_TotalWorkTime - (m_FinishTime - totalTime)) :
        m_TotalWorkTime;

    // std::cout << "Got " << (m_FinishTime);
    // std::cout << "Got " << (m_FinishTime < INF ? "true " : "false ");
    return realWorkTime / totalTime;
}


// std::ostream& getLoadCoeff(std::ostream& os, const SinglecoreDevice& device, double totalTime) {
//     const double realWorkTime = (device.m_FinishTime < INF) ?
//         // subtract the time that didn't get the chance to work
//         (device.m_TotalWorkTime - (device.m_FinishTime - totalTime)) :
//         device.m_TotalWorkTime;
//
//     os << (realWorkTime / totalTime);
//
//     return os;
// }


std::ostream& operator<<(std::ostream& os, const SinglecoreDevice& device) {
    os << "Awaiting: " << device.m_AmountOfAwaitingTasks << ". Finish: " << device.m_FinishTime;

    return os;
}
