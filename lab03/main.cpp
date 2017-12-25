#include <algorithm>
#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <map>
#include <stdlib.h> // simple random
#include "DeviceType.h"


// [[]] - array of rows
// v[row][column]

typedef std::vector<double> Vector;
typedef std::vector<Vector> Matrix;


struct Device {
    unsigned int waiting;
    unsigned int processing;
    unsigned int resource;

    Device() : Device(0, 0, 0) {}

    Device(unsigned int waiting, unsigned int processing, unsigned int resource) :
        waiting(waiting),
        processing(processing),
        resource(resource) {}

    Device clone() const {
        return Device(waiting, processing, resource);
    }

    void receiveTask() {
        waiting++;
    }

    void finishTask() {
        processing--;
        resource++;
    }

    void startIfCan() {
        while (waiting > 0 && resource > 0) {
            waiting--;
            resource--;
            processing++;
        }
    }

    bool isWorking() const {
        return processing > 0;
    }
};


typedef std::array<Device, DeviceType::AmountOfDeviceTypes> State;

struct Transition {
    const double tau;
    const double probability;
    const unsigned int destinationIndex;

    Transition(double tau, double probability, unsigned int destinationIndex) :
        tau(tau), probability(probability), destinationIndex(destinationIndex) {}
};
struct TransitionContinious {
    const double intensity;
    const unsigned int destinationIndex;

    TransitionContinious(double intensity, unsigned int destinationIndex) :
        intensity(intensity), destinationIndex(destinationIndex) {}
};

// index in StatesBank -> [conections from it]
typedef std::vector<std::vector<Transition>> ErreichbarkeitBaum;
typedef std::vector<std::vector<TransitionContinious>> ErreichbarkeitBaumContinious;

struct StatesBank {
    private:
        std::vector<State> states;

        bool equal(const Device& device1, const Device& device2) {
            return (
                    device1.waiting == device2.waiting &&
                    device1.processing == device2.processing &&
                    device1.resource == device2.resource
            );
        }

        bool equal(const State& state1, const State& state2) {
            for (unsigned int i = 0; i < DeviceType::AmountOfDeviceTypes; i++) {
                if (!equal(state1[i], state2[i])) {
                    return false;
                }
            }

            return true;
        }

    public:
        StatesBank() :
            states(std::vector<State>()) {}

        unsigned int processStateAndGetIndex(const State& state, ErreichbarkeitBaum& erreichbarkeitBaum) {
            unsigned int index = 0;
            for (const State& currentState : states) {
                if (equal(state, currentState)) {
                    return index;
                }
                index++;
            }

            // Such state does not exist yet => create
            states.push_back(state);
            // Create an empty ready slot
            // We hope that this push corresponds to the index 'index'
            erreichbarkeitBaum.push_back(
                    std::vector<Transition>()
            );

            return index;
        }

        unsigned int size() const {
            return states.size();
        }

        State get(unsigned int index) const {
            return states[index];
        }
};


// from -> [(to, probability)]
typedef std::map<DeviceType, std::vector<std::pair<DeviceType, double> > > DeviceTransitionMap;
// deviceType -> tau
typedef std::map<DeviceType, double> TauMap;


Vector solveSystemOfLinearEquations(
        const Matrix& A,
        const Vector& B);

double det(const Matrix& M);


DeviceTransitionMap initDeviceTransitionMap();
TauMap initTauMap();
State initStartingState(unsigned int amountOfTasksInSystem, unsigned int amountOfCores);

State clone(const State& state);
void spawnStatesForState(const State& state, StatesBank& statesBank, ErreichbarkeitBaum& erreichbarkeitBaum,
        const DeviceTransitionMap& deviceTransitionMap, const TauMap& tauMap);


void print(const Device& device) {
    std::cout << "<" << device.waiting << "," << device.processing << "," << device.resource << ">";
}
void print(const State& state) {
    std::cout << "{";
    for (const Device& device : state) {
        print(device);
        std::cout << " , ";
    }
    std::cout << "}" << std::endl;
}


// -------------------------------- main --------------------------------------


int main() {
    const unsigned int amountOfTasksInSystem = 3;
    // const unsigned int amountOfTasksInSystem = 5;
    const unsigned int amountOfCores = 1;
    // const unsigned int amountOfCores = 3;

    const DeviceTransitionMap deviceTransitionMap = initDeviceTransitionMap();
    const TauMap tauMap = initTauMap();

    StatesBank statesBank;
    ErreichbarkeitBaum erreichbarkeitBaum;
    statesBank.processStateAndGetIndex(
            initStartingState(amountOfTasksInSystem, amountOfCores),
            erreichbarkeitBaum);


    // Construct all possible states and connections for them
    {
        unsigned int previousStatesAmount = 0;
        unsigned int newStatesAmount = statesBank.size(); // == 1
        // While new states are still being added, need to check them as well
        while (previousStatesAmount < newStatesAmount) {
            // For each new State see if we can swap any even newer States
            for (unsigned int i = previousStatesAmount; i < newStatesAmount; i++) {
                const State state = statesBank.get(i);
                spawnStatesForState(state, statesBank, erreichbarkeitBaum, deviceTransitionMap, tauMap);
                std::cout << std::setw(5) << i << ": ";
                print(state);
            }

            previousStatesAmount = newStatesAmount;
            newStatesAmount = statesBank.size();
        }
    }

    // {
    //     unsigned int i = 0;
    //     for (const auto& state : erreichbarkeitBaum) {
    //         std::cout << "con " << i++ << ": " << std::endl;
    //         for (const auto& con : state) {
    //             std::cout << "    to " << con.destinationIndex << " < " << con.tau << " , " << con.probability << " >" << std::endl;
    //         }
    //     }
    // }


    std::cout << "-----------------------------" << std::endl;


    // Now we have a tree(ErreichbarkeitBaum) of connections marked with (tau, probability).
    // We need a tree of connections marked with (intensity)
    ErreichbarkeitBaumContinious erreichbarkeitBaumContinious;
    erreichbarkeitBaumContinious.reserve(erreichbarkeitBaum.size()); // same amount of source states
    {
        unsigned int stateIndex = 0;
        for (const auto& stateTransitions : erreichbarkeitBaum) {
            std::vector<TransitionContinious> newTransitions;
            std::vector<unsigned int> destinationIndicesAccountedFor;
            // for (const Transition& baseTransition : stateTransitions) {
            for (unsigned int baseTransitionIndex = 0; baseTransitionIndex < stateTransitions.size(); baseTransitionIndex++) {
                const Transition& baseTransition = stateTransitions[baseTransitionIndex];
                const unsigned int baseDestinationIndex = baseTransition.destinationIndex;

                if (baseDestinationIndex == stateIndex) {
                    // Allow no self-transitions
                    continue;
                }

                // Check if we already have new transition for that destination
                if (std::find(
                            destinationIndicesAccountedFor.begin(),
                            destinationIndicesAccountedFor.end(),
                            baseDestinationIndex)
                        != destinationIndicesAccountedFor.end()) {
                    // Such entry exists => already accounted for => skip
                    continue;
                }
                destinationIndicesAccountedFor.push_back(baseDestinationIndex);

                double intensity = baseTransition.probability * (1.0 / baseTransition.tau);
                // That ^ guy will be here for sure, but maybe someone else as well?
                // Check the remaining for ones with same destination(pretty unlikely)
                // (start from baseTransitionIndex+1 because all Transitions before it are already accounted for for sure)
                for (unsigned int i = baseTransitionIndex + 1; i < stateTransitions.size(); i++) {
                // for (const Transition& transition : stateTransitions) {
                    const Transition transition = stateTransitions[i];
                    if (transition.destinationIndex == baseDestinationIndex) {
                        // std::cout << "** Found same: index=" << baseDestinationIndex << std::endl;
                        // Have one more transition with same destination
                        intensity += transition.probability * (1.0 / transition.tau);
                    }
                }

                // std::cout << "    to " << baseDestinationIndex << " < " << intensity << " >" << std::endl;
                newTransitions.push_back(TransitionContinious(intensity, baseDestinationIndex));
            }

            erreichbarkeitBaumContinious.push_back(newTransitions);
            stateIndex++;
        }
    }


    // {
    //     unsigned int i = 0;
    //     for (const auto& state : erreichbarkeitBaumContinious) {
    //         std::cout << "con " << i++ << ": " << std::endl;
    //         for (const auto& con : state) {
    //             std::cout << "    to " << con.destinationIndex << " < " << con.intensity << " >" << std::endl;
    //         }
    //     }
    // }


    // Ooookey, so now we have a tree(Baum) of states and transitions between them
    // with corresponding intensities.
    // Now let's say that each State has certain probability P_i,
    // and let's write a system of linear equations that represents that tree(Baum).
    // Our goal is to derive each P_i and generate the following map: State <-> P.


    return 0;
}


// The State for which this function is call must already be in StatesBank
void spawnStatesForState(const State& state, StatesBank& statesBank, ErreichbarkeitBaum& erreichbarkeitBaum,
        const DeviceTransitionMap& deviceTransitionMap, const TauMap& tauMap) {
    for (unsigned int workingDeviceIndex = 0; workingDeviceIndex < DeviceType::AmountOfDeviceTypes; workingDeviceIndex++) {
        if (!state[workingDeviceIndex].isWorking()) {
            continue;
        }
        const unsigned int stateIndex =
            statesBank.processStateAndGetIndex(state, erreichbarkeitBaum); // already in bank
        const DeviceType workingDeviceType = static_cast<DeviceType>(workingDeviceIndex);

        // The blueprint state is not saved anywhere, because it is intermediate,
        // because the Task that finished(departed) is not regisered anywhere.
        // The Task gets registered for all States in the loop below, and they
        // are the ones that get saved eventually
        State blueprintState = clone(state);
        blueprintState[workingDeviceIndex].finishTask();
        blueprintState[workingDeviceIndex].startIfCan();

        // for each destination:
        // create copy of blueprint, receive with appropriate device, start it if can,
        // add that new state to the bank, add link to the Baum with probability and tau
        for (const auto& destination : deviceTransitionMap.at(workingDeviceType)) {
            State newState = clone(blueprintState);
            newState[destination.first].receiveTask();
            newState[destination.first].startIfCan();

            const double tau = tauMap.at(workingDeviceType);
            const double probability = destination.second;
            const unsigned int index = statesBank.processStateAndGetIndex(newState, erreichbarkeitBaum);

            erreichbarkeitBaum[stateIndex].push_back(
                { tau, probability, index }
            );
        }
    }
}


State clone(const State& state) {
    State newState;
    for (unsigned int i = 0; i < DeviceType::AmountOfDeviceTypes; i++) {
        newState[i] = state[i].clone();
    }

    return newState;
}


Vector solveSystemOfLinearEquations(
        const Matrix& A,
        const Vector& B) {
    const unsigned int N = A.size();
    Vector X;
    X.reserve(N);

    // Initialize once, reuse N times
    Matrix matrix;
    matrix.reserve(N);
    for (unsigned int i = 0; i < N; i++) matrix.push_back(Vector(N, 0.0));

    const double det0 = det(A);
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int row = 0; row < N; row++) {
            for (unsigned int column = 0; column < N; column++) {
                matrix[row][column] = (column == i) ? B[row] : A[row][column];
            }
        }

        X.push_back(det(matrix) / det0);
    }

    return X;
}


double det(const Matrix& M) {
    const unsigned int N = M.size();
    if (N == 2) {
        return (M[0][0] * M[1][1] - M[1][0] * M[0][1]);
    }

    double determinant = 0.0;
    int sign = +1;
    unsigned int elemIndex = 0;
    for (const double elem : M[0]) {
        Matrix subMatrix;
        subMatrix.reserve(N - 1);

        for (unsigned int rowIndex = 1; rowIndex < N; rowIndex++) {
            Vector row;
            row.reserve(N - 1);

            for (unsigned int columnIndex = 0; columnIndex < N; columnIndex++) {
                if (columnIndex == elemIndex) {
                    continue;
                }
                row.push_back(M[rowIndex][columnIndex]);
            }

            subMatrix.push_back(row);
        }

        determinant += sign * elem * det(subMatrix);

        sign *= -1;
        elemIndex++;
    }

    return determinant;
}


State initStartingState(unsigned int amountOfTasksInSystem, unsigned int amountOfCores) {
    State state;

    // Initialize with devices
    const unsigned int amountOfDeviceTypes = DeviceType::AmountOfDeviceTypes;
    for (unsigned int i = 0; i < amountOfDeviceTypes; i++) {
        const bool isCpu = static_cast<DeviceType>(i) == DeviceType::CPU;
        state[i] = Device(0, 0, isCpu ? amountOfCores : 1); // either 1 or amountOfCores resources
    }

    // Populate with tasks
    // srand (time(nullptr));
    // for (unsigned int i = 0; i < amountOfTasksInSystem; i++) {
    //     const unsigned int deviceIndex = rand() % amountOfDeviceTypes;
    //     state[deviceIndex].receiveTask();
    //     state[deviceIndex].startIfCan();
    // }
    state[0].receiveTask();
    state[0].receiveTask();
    state[2].receiveTask();
    state[2].startIfCan();
    state[0].startIfCan();

    return state;
}


TauMap initTauMap() {
    TauMap tauMap;

    tauMap.emplace(DeviceType::CPU, 1.1);
    tauMap.emplace(DeviceType::GPU, 2.2);
    tauMap.emplace(DeviceType::NB, 3.3);

    // tauMap.emplace(DeviceType::CPU, 2.0);
    // tauMap.emplace(DeviceType::GPU, 1.0);
    // tauMap.emplace(DeviceType::NB, 0.5);
    // tauMap.emplace(DeviceType::RAM, 10.0);
    // tauMap.emplace(DeviceType::VPU, 100.0);
    // tauMap.emplace(DeviceType::AP, 200.0);
    // tauMap.emplace(DeviceType::SB, 2.0);
    // tauMap.emplace(DeviceType::MA, 20.0);
    // tauMap.emplace(DeviceType::DC, 40.0);

    return tauMap;
}


DeviceTransitionMap initDeviceTransitionMap() {
    DeviceTransitionMap transitionMap;

    transitionMap.emplace(DeviceType::CPU,
        std::vector<std::pair<DeviceType, double>> {
            { DeviceType::CPU, 0.4 },
            { DeviceType::GPU, 0.6 }
        }
    );
    transitionMap.emplace(DeviceType::GPU,
        std::vector<std::pair<DeviceType, double>> {
            { DeviceType::NB, 0.2 },
            { DeviceType::CPU, 0.8 }
        }
    );
    transitionMap.emplace(DeviceType::NB,
        std::vector<std::pair<DeviceType, double>> {
            { DeviceType::GPU, 0.3 },
            { DeviceType::NB, 0.7 }
        }
    );

    // transitionMap.emplace(DeviceType::CPU,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::NB, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::GPU,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::NB, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::NB,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::CPU, 0.5 },
    //         { DeviceType::RAM, 0.4 },
    //         { DeviceType::SB, 0.05 },
    //         { DeviceType::GPU, 0.05 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::RAM,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::NB, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::VPU,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::CPU, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::AP,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::CPU, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::SB,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::NB, 0.5 },
    //         { DeviceType::DC, 0.3 },
    //         { DeviceType::MA, 0.1 },
    //         { DeviceType::VPU, 0.05 },
    //         { DeviceType::AP, 0.05 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::MA,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::SB, 1.0 }
    //     }
    // );
    // transitionMap.emplace(DeviceType::DC,
    //     std::vector<std::pair<DeviceType, double>> {
    //         { DeviceType::SB, 1.0 }
    //     }
    // );

    return transitionMap;
}
