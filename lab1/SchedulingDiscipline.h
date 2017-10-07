#ifndef SD_H
#define SD_H

#include <queue>
#include <vector>
#include "Task.h"


#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_QUEUE 1
#define LOG_LEVEL_ALL 2

#define LOG_LEVEL LOG_LEVEL_NONE

#define LOG(stuff, level) if (LOG_LEVEL >= level) {(stuff);}


class SchedulingDiscipline {
    public:
        SchedulingDiscipline(double lambda,
                double mu1, double mu2, double mu3,
                double hyperExponentialProbability1,
                double hyperExponentialProbability2);

        virtual ~SchedulingDiscipline();

    protected:
        const double m_LAMBDA;
        const double m_MU1;
        const double m_MU2;
        const double m_MU3;
        const double m_HyperExponentialProbability1;
        const double m_HyperExponentialProbability2;

        std::vector<Task*> m_FinishedTasks;

        Task* m_TaskOnProcessor;

    public:
        virtual std::vector<Task*> simulate(const unsigned int TASKS_TO_SIMULATE) = 0;

    protected:
        virtual Task* getTaskFromQueue() = 0;
        bool isProcessorBusy() const;
        bool isFirstMin(double a, double b) const;
        double min(double a, double b) const;

        double generatePoissonTime(double intencity) const;
        double generateHyperExponential() const;
        double generateUniformRandom() const;
};

#endif
