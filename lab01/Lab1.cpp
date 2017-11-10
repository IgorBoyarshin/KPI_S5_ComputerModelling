#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include "SchedulingDisciplinePER.h"
#include "SchedulingDisciplineFIFO.h"

double getAverageTimeInSystem(const std::vector<Task*>& tasks) {
    double totalTime = 0.0;
    for (const auto& task : tasks) {
        totalTime += task->getTotalTimeInSystem();
    }

    return (totalTime / static_cast<double>(tasks.size()));
}

double getDispersionOfTimeInSystem(
        const std::vector<Task*>& tasks, double averageTimeInSystem) {
    double sum = 0.0;
    for (const auto& task : tasks) {
        const double value = task->getTotalTimeInSystem() - averageTimeInSystem;
        sum += value * value;
    }

    return sum / (static_cast<double>(tasks.size()) - 1.0);
}

double getDispersionOfTimeInSystem(const std::vector<Task*>& tasks) {
    return getDispersionOfTimeInSystem(tasks, getAverageTimeInSystem(tasks));
}

double getAverageSystemResponseTime(const std::vector<Task*>& tasks) {
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
        double coeff1, double coeff2, double coeff3,
        unsigned int repetitions) {
    const double QUANTA_STEP = 0.1;
    const double QUANTA_LOWER_BOUND = 0.1;
    const double QUANTA_HIGHER_BOUND = 10.0;
    std::vector<double> stats;

    for (double quanta = QUANTA_LOWER_BOUND; quanta <= QUANTA_HIGHER_BOUND; quanta += QUANTA_STEP) {
        double averageTimeInSystem = 0.0;
        double dispersionOfTimeInSystem = 0.0;
        double averageSystemResponseTime = 0.0;

        for (unsigned int i = 0; i < repetitions; i++) {
            SchedulingDisciplinePER sd(
                lambda, mu1, mu2, mu3,
                hyperExponentialProbability1,
                hyperExponentialProbability2,
                quanta);
            const std::vector<Task*> finishedTasks =
                sd.simulate(tasksToSimulate);

            averageTimeInSystem +=
                getAverageTimeInSystem(finishedTasks);
            dispersionOfTimeInSystem +=
                getDispersionOfTimeInSystem(finishedTasks, averageTimeInSystem);
            averageSystemResponseTime +=
                getAverageSystemResponseTime(finishedTasks);

            deleteFinishedTasks(finishedTasks);
        }

        averageTimeInSystem /= 1.0 * repetitions;
        dispersionOfTimeInSystem /= 1.0 * repetitions;
        averageSystemResponseTime /= 1.0 * repetitions;

        stats.push_back(
            coeff1 * averageTimeInSystem +
            coeff2 * dispersionOfTimeInSystem +
            coeff3 * averageSystemResponseTime);
    }


    // std::map<double, double> hist;
    // unsigned int i = 0;
    // for (double q = QUANTA_LOWER_BOUND; q <= QUANTA_HIGHER_BOUND; q += QUANTA_STEP) {
    //     hist[q] = stats[i++];
    // }
    //
    // for (auto p : hist) {
    //     std::cout << std::fixed << std::setprecision(3) << std::setw(5)
    //               << p.first << ' '
    //               << std::string(static_cast<unsigned int>(p.second*10.0), '*')
    //               << '\n';
    // }

    // Now find max index
    unsigned int index = 0;
    unsigned int indexOfMax = 0;
    double currentMax = stats[0];
    for (double value : stats) {
        if (value > currentMax) {
            currentMax = value;
            indexOfMax = index;
        }
        index++;
    }
    const double BEST_QUANTA =
        QUANTA_LOWER_BOUND + QUANTA_STEP * indexOfMax;

    return BEST_QUANTA;
}

int main() {
    std::cout << "|------------------------------------" << std::endl;

    const double LAMBDA = 0.5;
    const double MU1 = 20.0;
    const double MU2 = 2.0;
    const double MU3 = 0.1 / 0.059;
    const double HYPER_EXPONENTIAL_PROBABILITY_1 = 0.001;
    const double HYPER_EXPONENTIAL_PROBABILITY_2 = 0.94;
    const double QUANTA = 0.9;
    const unsigned int TASKS_TO_SIMULATE = 7000;

    // The results of the modelling are not stable enough, that is,
    // the dispersion jumps drastically from one execution to another.
    // The situation is improved as TASKS_TO_SIMULATE grows,
    // and the manually observed average of output values across different
    // executions is what we actually expect to see.
    // So lets perform a few(REPETITIONS) executions and average the outputs.
    // The new output are marvellous!
    const unsigned int REPETITIONS = 30;

    {
        double averageTimeInSystem = 0.0;
        double dispersionOfTimeInSystem = 0.0;
        double averageSystemResponseTime = 0.0;

        for (unsigned int i = 0; i < REPETITIONS; i++) {
            SchedulingDisciplinePER sd(
                LAMBDA, MU1, MU2, MU3,
                HYPER_EXPONENTIAL_PROBABILITY_1,
                HYPER_EXPONENTIAL_PROBABILITY_2,
                QUANTA);
            const std::vector<Task*> finishedTasks =
                sd.simulate(TASKS_TO_SIMULATE);

            averageTimeInSystem += getAverageTimeInSystem(finishedTasks);
            dispersionOfTimeInSystem += getDispersionOfTimeInSystem(finishedTasks);
            averageSystemResponseTime += getAverageSystemResponseTime(finishedTasks);

            deleteFinishedTasks(finishedTasks);
        }

        averageTimeInSystem /= 1.0 * REPETITIONS;
        dispersionOfTimeInSystem /= 1.0 * REPETITIONS;
        averageSystemResponseTime /= 1.0 * REPETITIONS;


        std::cout << "      PER:" << std::endl;
        std::cout << "Average time in system = "
            << averageTimeInSystem << std::endl;
        std::cout << "Dispersion of time in system = "
            << dispersionOfTimeInSystem << std::endl;
        std::cout << "Average system response time = "
            << averageSystemResponseTime << std::endl;
    }

    std::cout << "|------------------------------------" << std::endl;

    {
        double averageTimeInSystem = 0.0;
        double dispersionOfTimeInSystem = 0.0;
        double averageSystemResponseTime = 0.0;

        for (unsigned int i = 0; i < REPETITIONS; i++) {
            SchedulingDisciplineFIFO sd(
                LAMBDA, MU1, MU2, MU3,
                HYPER_EXPONENTIAL_PROBABILITY_1,
                HYPER_EXPONENTIAL_PROBABILITY_2);
            const std::vector<Task*> finishedTasks =
                sd.simulate(TASKS_TO_SIMULATE);

            averageTimeInSystem += getAverageTimeInSystem(finishedTasks);
            dispersionOfTimeInSystem += getDispersionOfTimeInSystem(finishedTasks);
            averageSystemResponseTime += getAverageSystemResponseTime(finishedTasks);

            deleteFinishedTasks(finishedTasks);
        }

        averageTimeInSystem /= 1.0 * REPETITIONS;
        dispersionOfTimeInSystem /= 1.0 * REPETITIONS;
        averageSystemResponseTime /= 1.0 * REPETITIONS;

        std::cout << "      FIFO:" << std::endl;
        std::cout << "Average time in system = "
            << averageTimeInSystem << std::endl;
        std::cout << "Dispersion of time in system = "
            << dispersionOfTimeInSystem << std::endl;
        std::cout << "Average system response time = "
            << averageSystemResponseTime << std::endl;
    }

    std::cout << "|------------------------------------" << std::endl;

    // std::cout << "Best quanta:" << findBestQuanta(LAMBDA, MU1, MU2, MU3,
    //         HYPER_EXPONENTIAL_PROBABILITY_1,
    //         HYPER_EXPONENTIAL_PROBABILITY_2,
    //         TASKS_TO_SIMULATE,
    //         -7.0, -1.0, 0.0, REPETITIONS) << std::endl;

    return 0;
}
