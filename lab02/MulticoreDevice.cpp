#include "MulticoreDevice.h"


MulticoreDevice::MulticoreDevice(
        double tau,
        std::map<DeviceType, double> transitionMap, unsigned int amountOfCores) :
        Device(tau, transitionMap),
        m_AmountOfCores(amountOfCores),
        m_CoresFinishTime(std::vector<double>(m_AmountOfCores, INF)),
        m_CoresTotalWorkTime(std::vector<double>(m_AmountOfCores, 0.0)) {}


void MulticoreDevice::startIfCan(double currentGlobalTime) {
    if (m_AmountOfAwaitingTasks == 0) {
        // No awaiting tasks => nothing to start
        return;
    }

    // Start all tasks on all available cores
    unsigned int indexOfAvailableCore = findAvailableCore();
    while (m_AmountOfAwaitingTasks > 0 && indexOfAvailableCore != m_AmountOfCores) {
        // Have an available task and an available core => start
        const double coreWorkTime = generateWorkTime();
        m_CoresTotalWorkTime[indexOfAvailableCore] += coreWorkTime;
        m_CoresFinishTime[indexOfAvailableCore] = currentGlobalTime + coreWorkTime;
        m_AmountOfAwaitingTasks--;

        indexOfAvailableCore = findAvailableCore();
    }
}


unsigned int MulticoreDevice::findAvailableCore() const {
    unsigned int i;
    for (i = 0; i < m_AmountOfCores; i++) {
        if (m_CoresFinishTime[i] == INF) {
            return i;
        }
    }

    return i;
}


unsigned int MulticoreDevice::findCoreWithMinFinishTime() const {
    double minTime = m_CoresFinishTime[0];
    unsigned int indexOfMinTime = 0;
    for (unsigned int i = 1; i < m_AmountOfCores; i++) {
        const double coreFinishTime = m_CoresFinishTime[i];
        if (coreFinishTime < minTime) {
            minTime = coreFinishTime;
            indexOfMinTime = i;
        }
    }

    return indexOfMinTime;
}


double MulticoreDevice::generateWorkTime() const {
    // CPU has constant tau
    return m_Tau;
}


double MulticoreDevice::getFinishTime() const {
    return m_CoresFinishTime[findCoreWithMinFinishTime()];
}


DeviceType MulticoreDevice::finishTask() {
    m_CoresFinishTime[findCoreWithMinFinishTime()] = INF;

    return pickDestinationDeviceType();
}


std::vector<double> MulticoreDevice::getLoadCoeffs(double totalTime) const {
    std::vector<double> coresLoadCoeffs;
    for (unsigned int coreIndex = 0; coreIndex < m_AmountOfCores; coreIndex++) {
        const double realWorkTime = (m_CoresFinishTime[coreIndex] < INF) ?
            // subtract the time that didn't get the chance to work
            (m_CoresTotalWorkTime[coreIndex] - (m_CoresFinishTime[coreIndex] - totalTime)) :
            m_CoresTotalWorkTime[coreIndex];

        coresLoadCoeffs.push_back(realWorkTime / totalTime);
    }

    return coresLoadCoeffs;
}
