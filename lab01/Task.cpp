#include "Task.h"


Task::Task(double startTime, double timeToSolve) :
    m_StartTime(startTime), m_TimeOfSystemResponce(-1.0),
    m_TimeToSolve(timeToSolve), m_TimeLeftToSolve(timeToSolve),
    m_FinishTime(-1.0) {}

double Task::getTimeToSolve() const {
    return m_TimeToSolve;
}


double Task::getTimeLeftToSolve() const {
    return m_TimeLeftToSolve;
}


bool Task::process(double time) {
    m_TimeLeftToSolve -= time;
    if (m_TimeLeftToSolve <= 0.0) {
        m_TimeLeftToSolve = 0.0;
        return true;
    }

    return false;
}


void Task::finish(double finishTime) {
    m_TimeLeftToSolve = 0.0;
    m_FinishTime = finishTime;
}


void Task::setFinishTime(double finishTime) {
    m_FinishTime = finishTime;
}


void Task::setTimeOfSystemResponce(double time) {
    if (m_TimeOfSystemResponce < 0.0) {
        m_TimeOfSystemResponce = time;
    }
}


double Task::getSystemResponseTime() const {
    return m_TimeOfSystemResponce - m_StartTime;
}


double Task::getTotalTimeInSystem() const {
    return m_FinishTime - m_StartTime;
}
