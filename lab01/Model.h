#ifndef MODEL_H
#define MODEL_H

class Model {
    public:
        Model(double lambda, double mu);

        void simulate(const unsigned int TASKS_TO_SIMULATE);
    private:
        const double m_LAMBDA;
        const double m_MU;

    private:
        isProcessor
        bool isFirstMin(double a, double b) const;
        double min(double a, double b) const;

        double generatePoissonTime(double intencity) const;
        double generateUniformRandom() const;
};

#endif
