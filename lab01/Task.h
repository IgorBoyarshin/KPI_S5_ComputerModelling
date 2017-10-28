#ifndef TASK_H
#define TASK_H

class Task {
    public:
        Task(double startTime, double timeToSolve);

    private:
        const double m_StartTime;
        double m_TimeOfSystemResponce; // first time it got processed
        const double m_TimeToSolve; // time required to solve it
        double m_TimeLeftToSolve;
        double m_FinishTime;

    public:
        double getTimeToSolve() const;
        double getTimeLeftToSolve() const;

        // processes the task for the specified amount of time
        // delta time
        // returns true if the task has finished
        bool process(double time);

        // finish immediately
        // absolute time
        void finish(double finishTime);

        // absolute time
        // updates the variable only if it hasn't beed set already
        void setTimeOfSystemResponce(double time);

        // delta time
        double getSystemResponseTime() const;

        // absolute time
        void setFinishTime(double finishTime);

        // delta time
        double getTotalTimeInSystem() const;
};

#endif
