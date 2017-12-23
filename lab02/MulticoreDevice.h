#ifndef MULTICOREDEVICE_H
#define MULTICOREDEVICE_H

#include <vector>
#include "Device.h"


class MulticoreDevice : public Device {
    public:
        MulticoreDevice(
                double tau,
                std::map<DeviceType, double> transitionMap,
                unsigned int amountOfCores);

    private:
        const unsigned int m_AmountOfCores;

        std::vector<double> m_CoresFinishTime;

        // index === core index
        std::vector<double> m_CoresTotalWorkTime;


    public:
        std::vector<double> getLoadCoeffs(double totalTime) const;

        void startIfCan(double currentGlobalTime);

        DeviceType finishTask();

        double getFinishTime() const;
        // std::string getLoadCoeff(double totalTime) const;


    protected:
        // TODO: Need 'virtual' here??
        double generateWorkTime() const;

    private:
        // returns m_AmountOfCores if all cores are busy
        unsigned int findAvailableCore() const;
        unsigned int findCoreWithMinFinishTime() const;
};


// std::ostream& operator<<(std::ostream& os, MulticoreDevice multicoreDevice);

#endif
