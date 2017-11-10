#include "SchedulingDisciplinePER.h"
#include <iostream>
#include <limits>


SchedulingDisciplinePER::SchedulingDisciplinePER(
    double lambda, double mu1, double mu2, double mu3,
    double hyperExponentialProbability1,
    double hyperExponentialProbability2,
    double quanta) :
    SchedulingDiscipline(
            lambda, mu1, mu2, mu3,
            hyperExponentialProbability1, hyperExponentialProbability2),
    m_QUANTA(quanta) {}


SchedulingDisciplinePER::~SchedulingDisciplinePER() {
    while (m_QueueMain.size() > 0) {
        Task* task = m_QueueMain.front();
        m_QueueMain.pop();
        delete task;
    }

    while (m_QueueSecondary.size() > 0) {
        Task* task = m_QueueSecondary.front();
        m_QueueSecondary.pop();
        delete task;
    }
}


std::vector<Task*> SchedulingDisciplinePER::simulate(const unsigned int TASKS_TO_SIMULATE) {
    constexpr double INF = std::numeric_limits<double>::infinity();
    double T = 0.0; // current time
    double t1 = 0.0; // new task arrival time
    double t2 = INF; // current task finish time
    unsigned int simulatedTasks = 0;

    while (simulatedTasks < TASKS_TO_SIMULATE) {
        LOG(
            std::cout << "Main: " << m_QueueMain.size()
                << "; Secondary: " << m_QueueSecondary.size() << std::endl
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

            double timeToSolve;
            // Put a different kind of task 10% of the time
            if (simulatedTasks % 10 == 0) {
                // timeToSolve = generatePoissonTime(0.1);
                // timeToSolve = 0.5;
                timeToSolve = generateHyperExponential();
            } else {
                timeToSolve = generateHyperExponential();
            }

            LOG(
                std::cout << "  It has " << timeToSolve << " solveTime" << std::endl
            , LOG_LEVEL_ALL)

            Task* newTask = new Task(T, timeToSolve);
            simulatedTasks++;
            if (isProcessorBusy()) {
                LOG(
                    std::cout << "  Processor busy => into main queue" << std::endl
                , LOG_LEVEL_ALL)

                m_QueueMain.push(newTask);
            } else {
                LOG(
                    std::cout << "  Processor free => process" << std::endl
                , LOG_LEVEL_ALL)

                newTask->setTimeOfSystemResponce(T);
                m_TaskOnProcessor = newTask;

                // What happens first:
                // the task finishes or the quanta expires
                const double timeOnProcessor = min(timeToSolve, m_QUANTA);
                t2 = T + timeOnProcessor;

                LOG(
                    std::cout << "  Setting t2 = T + timeOnProcessor = " << T
                        << " + " << timeOnProcessor
                        << (isFirstMin(timeToSolve, m_QUANTA) ? " (self)" : " (quanta)")
                        << " = " << t2 << std::endl
                , LOG_LEVEL_ALL)
            }

            // The task is in its place,
            // now prepare for the next task
            t1 = T + generatePoissonTime(m_LAMBDA);

            LOG(
                std::cout << "  Setting t1 = T + genPoissonTime = " << T
                    << " + " << (t1 - T) << " = " << t1 << std::endl
                , LOG_LEVEL_ALL)

        } else { // current task stop
            LOG(
                std::cout << "  t2 is min => task stop" << std::endl
            , LOG_LEVEL_ALL)

            // always decrease by quanta,
            // it will get pumped back to 0.0 inside(in case the task finished
            // on its own, not due to quanta expiration),
            // we potentially avoid some rounding problems this way
            const bool taskFinished = m_TaskOnProcessor->process(m_QUANTA);
            if (taskFinished) {
                LOG(
                    std::cout << "  Task finished. Into finished queue("
                        << m_FinishedTasks.size() << ")"<< std::endl
                , LOG_LEVEL_ALL)

                m_TaskOnProcessor->setFinishTime(T);
                m_FinishedTasks.push_back(m_TaskOnProcessor);
            } else {
                LOG(
                    std::cout << "  Task stopped. Into second queue" << std::endl
                , LOG_LEVEL_ALL)

                m_QueueSecondary.push(m_TaskOnProcessor);
            }

            // update task on processor from queue
            // will return nullptr is there is no task available
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
                // will update the timeOfSystemResponce only if it
                // hasn't beed set already.
                // It may have been set before if the getTaskFromQueue just
                // retrieved the next task from the secondary queue.
                m_TaskOnProcessor->setTimeOfSystemResponce(T);

                const double timeOnProcessor =
                    min(m_TaskOnProcessor->getTimeLeftToSolve(), m_QUANTA);
                t2 = T + timeOnProcessor;

                LOG(
                    std::cout << "  Left to solve: "
                        << m_TaskOnProcessor->getTimeLeftToSolve() << " / "
                        << m_TaskOnProcessor->getTimeToSolve() << std::endl
                        << "  Setting t2 = T + timeOnProcessor = " << T
                        << " + " << timeOnProcessor
                        << (isFirstMin(m_TaskOnProcessor->getTimeLeftToSolve(), m_QUANTA) ? " (self)" : " (quanta)")
                        << " = " << t2 << std::endl
                , LOG_LEVEL_ALL)
            }
        }
    }

    // std::cout << "Finished: " << m_FinishedTasks.size() << std::endl;
    return m_FinishedTasks;
}

Task* SchedulingDisciplinePER::getTaskFromQueue() {
    Task* nextTask = nullptr;

    if (m_QueueMain.size() > 0) {
        nextTask = m_QueueMain.front();
        m_QueueMain.pop();
    } else if (m_QueueSecondary.size() > 0) {
        nextTask = m_QueueSecondary.front();
        m_QueueSecondary.pop();
    }

    return nextTask;
}
