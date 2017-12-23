#include "Device.h"


Device::Device(
        // DeviceType type,
        double tau,
        std::map<DeviceType, double> transitionMap) :
        // m_Type(type),
        m_Tau(tau),
        m_TransitionMap(transitionMap),
        m_AmountOfAwaitingTasks(0),
        m_AmountOfAvailableResource(1) {}


void Device::receiveTask() {
    m_AmountOfAwaitingTasks++;
}


double Device::generateUniformRandom() const {
    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(e2);
}


double Device::generatePoissonTime(double tau) const {
    return (-1.0 * tau * log(generateUniformRandom()));
}


DeviceType Device::pickDestinationDeviceType() const {
    const double r = generateUniformRandom();
    double acc = 0.0;
    for (const auto& entry : m_TransitionMap) {
        acc += entry.second;
        if (r <= acc) {
            return entry.first;
        }
    }

    std::cout << "--- IMPOSSIBLE ---" << std::endl;
    // can't happen!! But just in case it does, return first DeviceType
    return m_TransitionMap.begin()->first;
}


// std::ostream& operator<<(std::ostream& os, const Device& device) {
// }
