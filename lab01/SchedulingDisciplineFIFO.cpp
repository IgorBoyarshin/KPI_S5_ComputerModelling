#include "SchedulingDisciplineFIFO.h"
#include <iostream>
#include <limits>


SchedulingDisciplineFIFO::SchedulingDisciplineFIFO(
    double lambda, double mu1, double mu2, double mu3,
    double hyperExponentialProbability1,
    double hyperExponentialProbability2) :
    SchedulingDiscipline(
            lambda, mu1, mu2, mu3,
            hyperExponentialProbability1, hyperExponentialProbability2) {}


SchedulingDisciplineFIFO::~SchedulingDisciplineFIFO() {
    while (m_Queue.size() > 0) {
        Task* task = m_Queue.front();
        m_Queue.pop();
        delete task;
    }
}


std::vector<Task*> SchedulingDisciplineFIFO::simulate(const unsigned int TASKS_TO_SIMULATE) {
    constexpr double INF = std::numeric_limits<double>::infinity();
    double T = 0.0; // current time
    double t1 = 0.0; // new task arrival time
    double t2 = INF; // current task finish time
    unsigned int simulatedTasks = 0;

    while (simulatedTasks < TASKS_TO_SIMULATE) {
        LOG(
            std::cout << "In queue: " << m_Queue.size() << std::endl
        , LOG_LEVEL_QUEUE)

        T = min(t1, t2);

        LOG(
            std::cout << "  t1 = " << t1 << "; t2 = " << t2 << std::endl
                << "  T jumped to " << T << std::endl
        , LOG_LEVEL_ALL)

        if (isFirstMin(t1, t2)) { // new task arrival(birth)
            LOG(
                std::cout << "  t1 is min => task arrival" << std::endl
            , LOG_LEVEL_ALL)

            const double timeToSolve = generateHyperExponential();

            LOG(
                std::cout << "  It has " << timeToSolve << " solveTime" << std::endl
            , LOG_LEVEL_ALL)

            Task* newTask = new Task(T, timeToSolve);
            simulatedTasks++;
            if (isProcessorBusy()) {
                LOG(
                    std::cout << "  Processor busy => into queue" << std::endl
                , LOG_LEVEL_ALL)

                m_Queue.push(newTask);
            } else {
                LOG(
                    std::cout << "  Processor free => process" << std::endl
                , LOG_LEVEL_ALL)

                newTask->setTimeOfSystemResponce(T);
                m_TaskOnProcessor = newTask;

                t2 = T + timeToSolve;

                LOG(
                    std::cout << "  Setting t2 = T + timeToSolve = " << T
                        << " + " << timeToSolve
                        << " = " << t2 << std::endl
                , LOG_LEVEL_ALL)
            }

            t1 = T + generatePoissonTime(m_LAMBDA);

            LOG(
                std::cout << "  Setting t1 = T + genPoissonTime = " << T
                    << " + " << (t1 - T) << " = " << t1 << std::endl
                , LOG_LEVEL_ALL)

        } else { // current task stop
            LOG(
                std::cout << "  t2 is min => task stop" << std::endl
            , LOG_LEVEL_ALL)

            m_TaskOnProcessor->finish(T);
            m_FinishedTasks.push_back(m_TaskOnProcessor);
            LOG(
                std::cout << "  Task finished. Into finished queue("
                    << m_FinishedTasks.size() << ")"<< std::endl
            , LOG_LEVEL_ALL)

            // update task on processor from queue.
            // returns nullptr if there is no task available
            m_TaskOnProcessor = getTaskFromQueue();

            LOG(
                std::cout << "  Getting task from queue:" << std::endl
            , LOG_LEVEL_ALL)

            if (m_TaskOnProcessor == nullptr) { // queue is empty
                LOG(
                    std::cout << "  Queue is empty => waiting, t2 = INF" << std::endl
                , LOG_LEVEL_ALL)

                t2 = INF;
            } else { // got a new task to process
                // updates the variable only if it hasn't beed set already.
                // For FIFO this is always the first setting
                m_TaskOnProcessor->setTimeOfSystemResponce(T);

                const double timeToSolve = m_TaskOnProcessor->getTimeToSolve();
                t2 = T + timeToSolve;
                LOG(
                    std::cout << "  Left to solve: "
                        << "  Setting t2 = T + timeToSolve = " << T
                        << " + " << timeToSolve
                        << " = " << t2 << std::endl
                , LOG_LEVEL_ALL)
            }
        }
    }

    return m_FinishedTasks;
}

Task* SchedulingDisciplineFIFO::getTaskFromQueue() {
    Task* nextTask = nullptr;

    if (m_Queue.size() > 0) {
        nextTask = m_Queue.front();
        m_Queue.pop();
    }

    return nextTask;
}
