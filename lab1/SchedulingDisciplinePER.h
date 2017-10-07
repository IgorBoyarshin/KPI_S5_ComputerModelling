#ifndef SDPER_H
#define SDPER_H

#include <queue>
#include <vector>
#include "Task.h"
#include "SchedulingDiscipline.h"

class SchedulingDisciplinePER : public SchedulingDiscipline {
    public:
        SchedulingDisciplinePER(double lambda,
                double mu1, double mu2, double mu3,
                double hyperExponentialProbability1,
                double hyperExponentialProbability2,
                double quanta);

        ~SchedulingDisciplinePER();

    private:
        const double m_QUANTA;

        std::queue<Task*> m_QueueMain;
        std::queue<Task*> m_QueueSecondary;

    public:
        std::vector<Task*> simulate(const unsigned int TASKS_TO_SIMULATE);

    protected:
        Task* getTaskFromQueue();
};

#endif
