#ifndef SDFIFO_H
#define SDFIFO_H

#include <queue>
#include <vector>
#include "Task.h"
#include "SchedulingDiscipline.h"

class SchedulingDisciplineFIFO: public SchedulingDiscipline {
    public:
        SchedulingDisciplineFIFO(double lambda,
                double mu1, double mu2, double mu3,
                double hyperExponentialProbability1,
                double hyperExponentialProbability2);

        ~SchedulingDisciplineFIFO();

    protected:
        std::queue<Task*> m_Queue;

    public:
        std::vector<Task*> simulate(const unsigned int TASKS_TO_SIMULATE);

    protected:
        Task* getTaskFromQueue();
};

#endif
