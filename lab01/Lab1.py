import random
import math
import Queue


class SchedulingDiscipline:
    def __init__(self, lambd, mu1, mu2, mu3, hyper_exponential_prob1,
                 hyper_exponential_prob2):
        self.lambd = lambd
        self.mu1 = mu1
        self.mu2 = mu2
        self.mu3 = mu3
        self.hyper_exponential_prob1 = hyper_exponential_prob1
        self.hyper_exponential_prob2 = hyper_exponential_prob2
        self.task_on_processor = None

    def is_processor_busy(self):
        return self.task_on_processor is not None

    def is_first_min(self, a, b):
        return a < b

    def gen_uniform_random(self):
        return random.random()

    def gen_poisson_time(self, intencity):
        return (-1.0 / intencity * math.log(self.gen_uniform_random()))

    def gen_hyper_exponential_time(self):
        PROB = self.gen_uniform_random()
        if (PROB < self.hyper_exponential_prob1):
            return self.gen_poisson_time(self.mu1)
        elif (PROB < self.hyper_exponential_prob2):
            return self.gen_poisson_time(self.mu2)
        else:
            return self.gen_poisson_time(self.mu3)


class SchedulingDisciplinePER(SchedulingDiscipline):
    def __init__(self, lambd, mu1, mu2, mu3,
                 hyper_exponential_prob1, hyper_exponential_prob2, quanta):
        super().__init__(lambd, mu1, mu2, mu3,
                         hyper_exponential_prob1, hyper_exponential_prob2)
        self.quanta = quanta
        self.queue_main = Queue.Queue()
        self.queue_secondary = Queue.Queue()

    def simulate(self, TASKS_TO_SIMULATE):

    def get_task_from_queue(self):
        if not self.queue_main.empty():
            return self.queue_main.get()
        elif not self.queue_secondary.empty():
            return self.queue_secondary.get()
        else:
            return None


if __name__ == "__main__":
    SchedulingDisciplinePER(1.0, 2.0, 2.0, 2.0, 0.5, 0.3, 1.0)
