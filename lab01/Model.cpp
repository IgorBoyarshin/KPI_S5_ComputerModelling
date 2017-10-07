#include "Model.h"
#include <cstdlib> // rand
#include <ctime> // time() for srand
#include "math.h" // log
#include <limits>
#include <iostream>

Model::Model(double lambda, double mu) :
    m_LAMBDA(lambda), m_MU(mu) {
    srand (static_cast <unsigned> (time(0)));
}


void Model::simulate(const unsigned int TASKS_TO_SIMULATE) {
    constexpr double INF = std::numeric_limits<double>::infinity();
    double T = 0.0; // current time
    double t1 = 0.0; // new task arrival time
    double t2 = INF; // current task finish time
    unsigned int simulatedTasks = 0;
    
    while (simulatedTasks < TASKS_TO_SIMULATE) {
        std::cout << generatePoissonTime(m_MU) << std::endl;
        T = min(t1, t2);
        if (isFirstMin(t1, t2)) { // new task arrival
            const double timeToSolve = generatePoissonTime(m_MU);

        } else { // current task finish

        }
    }
}


bool Model::isFirstMin(double a, double b) const {
    return a < b;
}


double Model::min(double a, double b) const {
    return a < b ? a : b;
}


double Model::generatePoissonTime(double intencity) const {
    return (-1.0 / intencity * log(generateUniformRandom()));
}


double Model::generateUniformRandom() const {
    return static_cast <double> (rand()) / 
        (static_cast <double> (RAND_MAX) + 1.0);
}
