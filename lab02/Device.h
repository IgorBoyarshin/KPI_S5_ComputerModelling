#ifndef DEVICE_H
#define DEVICE_H

#include <iostream>
#include <map>
#include <random>
#include "math.h" // log
#include "DeviceType.h"


constexpr double INF = std::numeric_limits<double>::infinity();

class Device {
    public:
        Device(
            // DeviceType type,
            double tau,
            std::map<DeviceType, double> transitionMap);

    protected:
        // const DeviceType m_Type;

        // Used when generating new delta
        const double m_Tau;

        // destination <-> probability of such transition
        const std::map<DeviceType, double> m_TransitionMap;

        // Absolute task finish time
        // double m_FinishTime;

        unsigned int m_AmountOfAwaitingTasks;
        unsigned int m_AmountOfAvailableResource;


    public:
        // Just increment the amountOfAwaitingTasks
        void receiveTask();

        virtual void startIfCan(double currentGlobalTime) = 0;

        virtual double getFinishTime() const = 0;

        // also picks and returns destination
        virtual DeviceType finishTask() = 0;

        // friend std::ostream& operator<<(std::ostream& os, const Device& device);

        // virtual double getLoadCoeff(double totalTime) const;
        // virtual std::ostream& getLoadCoeff(std::ostream& os, double totalTime) const;

    protected:
        virtual double generateWorkTime() const = 0;

        double generatePoissonTime(double tau) const;
        // based on random
        DeviceType pickDestinationDeviceType() const;

    private:
        double generateUniformRandom() const;
};

// std::ostream& operator<<(std::ostream& os, const Device& device);

#endif
