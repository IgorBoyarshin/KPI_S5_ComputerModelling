#include "SchedulingDiscipline.h"
#include <iomanip>
#include <cstdlib> // rand
#include "math.h" // log
#include <iostream>
#include <map>
#include <random>


// TODO: most probably don't need to initialize m_FinishedTasks
SchedulingDiscipline::SchedulingDiscipline(
    double lambda, double mu1, double mu2, double mu3,
    double hyperExponentialProbability1,
    double hyperExponentialProbability2) :
    m_LAMBDA(lambda), m_MU1(mu1), m_MU2(mu2), m_MU3(mu3),
    m_HyperExponentialProbability1(hyperExponentialProbability1),
    m_HyperExponentialProbability2(hyperExponentialProbability2),
    m_FinishedTasks(std::vector<Task*>()), m_TaskOnProcessor(nullptr) {}


SchedulingDiscipline::~SchedulingDiscipline() {
    if (m_TaskOnProcessor) {
        delete m_TaskOnProcessor;
    }
}


bool SchedulingDiscipline::isProcessorBusy() const {
    return m_TaskOnProcessor != nullptr;
}


bool SchedulingDiscipline::isFirstMin(double a, double b) const {
    return a < b;
}


double SchedulingDiscipline::min(double a, double b) const {
    return a < b ? a : b;
}


double SchedulingDiscipline::generatePoissonTime(double intencity) const {
    return (-1.0 / intencity * log(generateUniformRandom()));
}


double SchedulingDiscipline::generateHyperExponential() const {
    const double probability = generateUniformRandom();
    if (probability < m_HyperExponentialProbability1) {
        return generatePoissonTime(m_MU1);
    } else if (probability < m_HyperExponentialProbability2) {
        return generatePoissonTime(m_MU2);
    } else {
        return generatePoissonTime(m_MU3);
    }
}


double SchedulingDiscipline::generateUniformRandom() const {
    static std::random_device rd;
    static std::mt19937 e2(rd());
    static std::uniform_real_distribution<double> dist(0.0, 1.0);

    return dist(e2);
}
