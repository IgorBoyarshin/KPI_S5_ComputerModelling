#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
// #include <ctime> // time() for srand
#include "SchedulingDisciplinePER.h"
#include "SchedulingDisciplineFIFO.h"

double getAverageTimeInSystem(const std::vector<Task*> tasks) {
    double totalTime = 0.0;
    for (const auto& task : tasks) {
        totalTime += task->getTotalTimeInSystem();
    }

    return (totalTime / static_cast<double>(tasks.size()));
}

double getDispersionOfTimeInSystem(
        const std::vector<Task*> tasks, double averageTimeInSystem) {
    double sum = 0.0;
    for (const auto& task : tasks) {
        const double value = task->getTotalTimeInSystem() - averageTimeInSystem;
        sum += value * value;
    }

    return sum / (static_cast<double>(tasks.size()) - 1.0);
}

double getAverageSystemResponseTime(const std::vector<Task*> tasks) {
    double totalTime = 0.0;
    for (const auto& task : tasks) {
        totalTime += task->getSystemResponseTime();
    }

    return (totalTime / static_cast<double>(tasks.size()));
}

void deleteFinishedTasks(const std::vector<Task*>& tasks) {
    for (auto& task : tasks) {
        delete task;
    }
}

double findBestQuanta(
        double lambda, double mu1, double mu2, double mu3,
        double hyperExponentialProbability1,
        double hyperExponentialProbability2,
        double tasksToSimulate,
        double coeff1, double coeff2, double coeff3) {
    const double QUANTA_STEP = 0.05;
    const double QUANTA_LOWER_BOUND = 0.05;
    const double QUANTA_HIGHER_BOUND = 5.0;
    std::vector<double> stats;

    for (double quanta = QUANTA_LOWER_BOUND; quanta <= QUANTA_HIGHER_BOUND; quanta += QUANTA_STEP) {
        SchedulingDisciplinePER sd(
                lambda, mu1, mu2, mu3,
                hyperExponentialProbability1,
                hyperExponentialProbability2,
                quanta);
        const std::vector<Task*> finishedTasks =
            sd.simulate(tasksToSimulate);

        const double averageTimeInSystem =
            getAverageTimeInSystem(finishedTasks);
        const double dispersionOfTimeInSystem =
            getDispersionOfTimeInSystem(finishedTasks, averageTimeInSystem);
        const double averageSystemResponseTime =
            getAverageSystemResponseTime(finishedTasks);

        stats.push_back(
                coeff1 * averageTimeInSystem +
                coeff2 * dispersionOfTimeInSystem +
                coeff3 * averageSystemResponseTime);
    }


    std::map<double, double> hist;
    unsigned int i = 0;
    for (double q = QUANTA_LOWER_BOUND; q <= QUANTA_HIGHER_BOUND; q += QUANTA_STEP) {
        hist[q] = stats[i++];
    }
    // for (int n = 0; n < 10000; ++n) {
    //     ++hist[std::floor(dist(e2))];
    // }

    for (auto p : hist) {
        std::cout << std::fixed << std::setprecision(1) << std::setw(4)
                  << p.first << ' ' << std::string(p.second*10, '*') << '\n';
    }

    // Now find max index
    unsigned int index = 0;
    unsigned int indexMax = 0;
    double currentMax = stats[0];
    for (double value : stats) {
        if (value > currentMax) {
            currentMax = value;
            indexMax = index;
        }
        index++;
    }
    const double BEST_QUANTA =
        QUANTA_LOWER_BOUND + QUANTA_STEP * indexMax;
    return BEST_QUANTA;
}

int main() {
    std::cout << "|------------------------------------" << std::endl;
    // srand (static_cast <unsigned> (time(0)));

    const double LAMBDA = 1.5;
    const double MU1 = 2.0;
    const double MU2 = 2.0;
    const double MU3 = 2.0;
    const double HYPER_EXPONENTIAL_PROBABILITY_1 = 0.5;
    const double HYPER_EXPONENTIAL_PROBABILITY_2 = 0.3;
    const double QUANTA = 0.9;
    const unsigned int TASKS_TO_SIMULATE = 10000;

    {
        std::cout << "      PER:" << std::endl;
        SchedulingDisciplinePER sd(
                LAMBDA, MU1, MU2, MU3,
                HYPER_EXPONENTIAL_PROBABILITY_1,
                HYPER_EXPONENTIAL_PROBABILITY_2,
                QUANTA);
        const std::vector<Task*> finishedTasks =
            sd.simulate(TASKS_TO_SIMULATE);

        std::cout << "Average time in system = "
            << getAverageTimeInSystem(finishedTasks) << std::endl;
        std::cout << "Dispersion of time in system = "
            << getDispersionOfTimeInSystem(finishedTasks,
                    getAverageTimeInSystem(finishedTasks)) << std::endl;
        std::cout << "Average system response time = "
            << getAverageSystemResponseTime(finishedTasks) << std::endl;

        deleteFinishedTasks(finishedTasks);
    }

    std::cout << "|------------------------------------" << std::endl;

    {
        std::cout << "      FIFO:" << std::endl;
        SchedulingDisciplineFIFO sd2(
                LAMBDA, MU1, MU2, MU3,
                HYPER_EXPONENTIAL_PROBABILITY_1,
                HYPER_EXPONENTIAL_PROBABILITY_2);
        const std::vector<Task*> finishedTasks =
            sd2.simulate(TASKS_TO_SIMULATE);

        std::cout << "Average time in system = "
            << getAverageTimeInSystem(finishedTasks) << std::endl;
        std::cout << "Dispersion of time in system = "
            << getDispersionOfTimeInSystem(finishedTasks,
                    getAverageTimeInSystem(finishedTasks)) << std::endl;
        std::cout << "Average system response time = "
            << getAverageSystemResponseTime(finishedTasks) << std::endl;

        deleteFinishedTasks(finishedTasks);
    }

    std::cout << "|------------------------------------" << std::endl;

    std::cout << "Best:" << findBestQuanta(LAMBDA, MU1, MU2, MU3,
            HYPER_EXPONENTIAL_PROBABILITY_1,
            HYPER_EXPONENTIAL_PROBABILITY_2,
            TASKS_TO_SIMULATE,
            -7.0, -1.0, 0.0) << std::endl;

    return 0;
}
