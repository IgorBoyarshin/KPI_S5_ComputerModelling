#ifndef SINGLECOREDEVICE_H
#define SINGLECOREDEVICE_H

#include "Device.h"


class SinglecoreDevice : public Device {
    public:
        SinglecoreDevice(
                // DeviceType type,
                double tau,
                std::map<DeviceType, double> transitionMap);

    private:
        double m_FinishTime;
        double m_TotalWorkTime;

    public:
        double getLoadCoeff(double totalTime) const;

        void startIfCan(double currentGlobalTime);
        double getFinishTime() const;
        DeviceType finishTask();

        friend std::ostream& operator<<(std::ostream& os, const SinglecoreDevice& device);
        // friend std::ostream& getLoadCoeff(std::ostream& os, const SinglecoreDevice& device, double totalTime);

    protected:
        double generateWorkTime() const;
};

std::ostream& operator<<(std::ostream& os, const SinglecoreDevice& device);

// std::ostream& getLoadCoeff(std::ostream& os, const SinglecoreDevice& device, double totalTime);


#endif
