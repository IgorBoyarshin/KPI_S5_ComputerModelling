#include "SchedulingDiscipline.h"
#include <cstdlib> // rand
#include "math.h" // log
#include <iostream>
#include <random>


SchedulingDiscipline::SchedulingDiscipline(
    double lambda, double mu1, double mu2, double mu3,
    double hyperExponentialProbability1,
    double hyperExponentialProbability2) :
    m_LAMBDA(lambda), m_MU1(mu1), m_MU2(mu2), m_MU3(mu3),
    m_HyperExponentialProbability1(hyperExponentialProbability1),
    m_HyperExponentialProbability2(hyperExponentialProbability2),
    m_TaskOnProcessor(nullptr) {

        // for (int i=0; i<20;i++){
        //     std::cout << generateUniformRandom() << std::endl;
        // }
}


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
    static std::uniform_real_distribution<> dist(0, 1);

    return dist(e2);
    // return static_cast <double> (rand()) /
    //     (static_cast <double> (RAND_MAX) + 1.0);
}
