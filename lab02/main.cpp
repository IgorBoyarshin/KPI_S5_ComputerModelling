#include <iostream>
#include <iomanip>
#include <map>
// #include <vector>
#include <limits> // INF
#include "Device.h"
#include "DeviceType.h"
#include "SinglecoreDevice.h"
#include "MulticoreDevice.h"
#include <stdlib.h> // simple random



// constexpr double INF = std::numeric_limits<double>::infinity();
typedef std::map<DeviceType, Device* const> DevicesMap;


Device* findDeviceWithMinFinishTime(const DevicesMap& devicesMap);
void initializeWithDevices(DevicesMap& devicesMap, unsigned int amountOfCpuCores);
void initializeDevicesWithTasks(DevicesMap& devicesMap, unsigned int amountOfTasks);

double simulate(
        DevicesMap& devicesMap,
        unsigned int amountOfEventsToSimulate,
        unsigned int amountOfTasksInSystem);



int main() {
    std::cout << "------------------ Begin ------------------" << std::endl;

    const unsigned int amountOfTasksInSystem = 5;
    const unsigned int amountOfEventsToSimulate = 1000000;
    const unsigned int amountOfCpuCores = 3;

    DevicesMap devicesMap;

    initializeWithDevices(devicesMap, amountOfCpuCores);
    initializeDevicesWithTasks(devicesMap, amountOfTasksInSystem);

    // for (const auto& entry : devicesMap) {
    //     const DeviceType deviceType = entry.first;
    //     if (deviceType == DeviceType::CPU) {
    //         // std::cout << entry.first << ": " << *((MulticoreDevice*)entry.second) << std::endl;
    //     } else {
    //         std::cout << entry.first << ": " << *((SinglecoreDevice*)entry.second) << std::endl;
    //     }
    // }

    const double totalTime = simulate(devicesMap, amountOfEventsToSimulate, amountOfTasksInSystem);
    std::cout << "Load coeffs:" << std::endl;
    for (const auto& entry : devicesMap) {
        const DeviceType deviceType = entry.first;
        if (deviceType == DeviceType::CPU) {
            const MulticoreDevice device = *((MulticoreDevice*) entry.second);
            std::vector<double> loadCoeffs = device.getLoadCoeffs(totalTime);

            unsigned int coreIndex = 0;
            for (const double loadCoeff : loadCoeffs) {
                std::cout << std::setw(4) << deviceType << " core " << coreIndex++ << ": " << loadCoeff << std::endl;
            }
        } else {
            const SinglecoreDevice device = *((SinglecoreDevice*) entry.second);
            double loadCoeff = device.getLoadCoeff(totalTime);

            std::cout << std::setw(11) << deviceType << ": " << loadCoeff << std::endl;
        }
    }
}


double simulate(
        DevicesMap& devicesMap,
        unsigned int amountOfEventsToSimulate,
        unsigned int amountOfTasksInSystem) {

    double currentGlobalTime = 0.0;
    for (unsigned int i = 0; i < amountOfEventsToSimulate; i++) {
        // Pick the device we're gonna be working with
        Device* const currentDevice =
            findDeviceWithMinFinishTime(devicesMap);
        currentGlobalTime = currentDevice->getFinishTime();

        // Finish the task on it, pick a destination and send the task there
        const DeviceType destination = currentDevice->finishTask(); // will set finishTime to INF
        // std::cout << "Goind to " << destination << std::endl;
        Device* const destinationDevice = devicesMap[destination];
        destinationDevice->receiveTask();

        // The change could only have influenced the source and the destination,
        // so we try to start only those two.
        currentDevice->startIfCan(currentGlobalTime);
        destinationDevice->startIfCan(currentGlobalTime);
        // for (auto& entry : devicesMap) {
        //     entry.second->startIfCan(currentGlobalTime);
        // }
    }

    return currentGlobalTime;
}


// Random
void initializeDevicesWithTasks(DevicesMap& devicesMap, unsigned int amountOfTasks) {
    srand (time(nullptr));
    const unsigned int amountOfDevices = devicesMap.size();
    for (unsigned int i = 0; i < amountOfTasks; i++) {
        const unsigned int index = rand() % amountOfDevices;
        Device* const randomDevice = devicesMap[static_cast<DeviceType>(index)];
        randomDevice->receiveTask();
        randomDevice->startIfCan(0.0);
    }
}


// Unless the amount of tasks in the system is 0,
// there will always be a Device with (< INF) finishTime
Device* findDeviceWithMinFinishTime(const DevicesMap& devicesMap) {
    double minFinishTime = INF;
    DeviceType minDeviceType;
    bool firstIteration = true;
    for (const auto& entry : devicesMap) {
        if (firstIteration) {
            minDeviceType = entry.first;
            firstIteration = false;
        }

        const double finishTime = entry.second->getFinishTime();
        if (finishTime < minFinishTime) {
            minFinishTime = finishTime;
            minDeviceType = entry.first;
        }
    }

    // std::cout <<"FOund " << minDeviceType << " with " << minFinishTime << std::endl;

    return devicesMap.at(minDeviceType);
}


void initializeWithDevices(DevicesMap& devicesMap, unsigned int amountOfCpuCores) {
    devicesMap.emplace(DeviceType::CPU, new MulticoreDevice(
        // DeciveType::CPU,
        2.0,
        {
            { DeviceType::NB, 1.0 }
        },
        amountOfCpuCores
    ));
    devicesMap.emplace(DeviceType::GPU, new SinglecoreDevice(
        // DeviceType::GPU,
        1.0,
        {
            { DeviceType::NB, 1.0 }
        }
    ));
    devicesMap.emplace(DeviceType::NB, new SinglecoreDevice(
        // DeviceType::NB,
        0.5,
        {
            { DeviceType::CPU, 0.5 },
            { DeviceType::RAM, 0.4 },
            { DeviceType::SB, 0.05 },
            { DeviceType::GPU, 0.05 }
        }
    ));
    devicesMap.emplace(DeviceType::RAM, new SinglecoreDevice(
        // DeviceType::RAM,
        10.0,
        {
            { DeviceType::NB, 1.0 }
        }
    ));
    devicesMap.emplace(DeviceType::VPU, new SinglecoreDevice(
        // DeviceType::VPU,
        100.0,
        {
            { DeviceType::CPU, 1.0 }
        }
    ));
    devicesMap.emplace(DeviceType::AP, new SinglecoreDevice(
        // DeviceType::AP,
        200.0,
        {
            { DeviceType::CPU, 1.0 }
        }
    ));
    devicesMap.emplace(DeviceType::SB, new SinglecoreDevice(
        // DeviceType::SB,
        2.0,
        {
            { DeviceType::NB, 0.5 },
            { DeviceType::DC, 0.3 },
            { DeviceType::MA, 0.1 },
            { DeviceType::VPU, 0.05 },
            { DeviceType::AP, 0.05 }
        }
    ));
    devicesMap.emplace(DeviceType::MA, new SinglecoreDevice(
        // DeviceType::MA,
        20.0,
        {
            { DeviceType::SB, 1.0 }
        }
    ));
    devicesMap.emplace(DeviceType::DC, new SinglecoreDevice(
        // DeviceType::DC,
        40.0,
        {
            { DeviceType::SB, 1.0 }
        }
    ));
}
