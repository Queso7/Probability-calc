#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <vector>
#include <cmath>
#include <string>
#include <stdexcept>
#include <algorithm>


inline long double mathFactorial(int n) {
    if (n < 0) return 0;
    long double res = 1.0;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}


inline double tgamma_approx(double x) {
    if (x < 0.5) return M_PI / (std::sin(M_PI * x) * tgamma_approx(1.0 - x));
    x -= 1.0;
    static const double p[] = {
        0.99999999999980993, 676.5203681218851, -1259.1392167224028,
        771.32342877765313, -176.61502916214059, 12.507381424447053,
        -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7
    };
    double g = 7.0;
    double a = p[0];
    for (int i = 1; i < 9; ++i) a += p[i] / (x + i);
    double t = x + g + 0.5;
    return std::sqrt(2.0 * M_PI) * std::pow(t, x + 0.5) * std::exp(-t) * a;
}

inline double gammp_approx(double a, double x) {
    if (x <= 0.0) return 0.0;
    double sum = 1.0 / a;
    double del = sum;
    double ap = a;
    for (int i = 1; i <= 100; ++i) {
        ap += 1.0;
        del *= x / ap;
        sum += del;
        if (std::abs(del) < std::abs(sum) * 3.0e-7) break;
    }
    return sum * std::exp(-x + a * std::log(x) - std::lgamma(a));
}

class Distribution {
public:
    virtual ~Distribution() = default;

    virtual double pdf(double x) const = 0;
    virtual double cdf(double x) const = 0;
    virtual double mean() const = 0;
    virtual double variance() const = 0;

    virtual double xMin() const = 0;
    virtual double xMax() const = 0;
    virtual bool isDiscrete() const = 0;
};


class BernoulliDistribution : public Distribution {
private:
    double m_p;
public:
    BernoulliDistribution(double p) : m_p(p) {
        if (m_p < 0.0 || m_p > 1.0) throw std::invalid_argument("p debe estar en [0, 1]");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k == 1) return m_p;
        if (k == 0) return 1.0 - m_p;
        return 0.0;
    }
    double cdf(double x) const override {
        if (x < 0.0) return 0.0;
        if (x < 1.0) return 1.0 - m_p;
        return 1.0;
    }
    double mean() const override { return m_p; }
    double variance() const override { return m_p * (1.0 - m_p); }
    double xMin() const override { return -0.5; }
    double xMax() const override { return 1.5; }
    bool isDiscrete() const override { return true; }
};

class BinomialDistribution : public Distribution {
private:
    int m_n; double m_p;
public:
    BinomialDistribution(int n, double p) : m_n(n), m_p(p) {
        if (m_n < 0) throw std::invalid_argument("n no puede ser negativo");
        if (m_p < 0.0 || m_p > 1.0) throw std::invalid_argument("p debe estar en [0, 1]");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k < 0 || k > m_n) return 0.0;
        long double nCr = mathFactorial(m_n) / (mathFactorial(k) * mathFactorial(m_n - k));
        return nCr * std::pow(m_p, k) * std::pow(1.0 - m_p, m_n - k);
    }
    double cdf(double x) const override {
        if (x < 0.0) return 0.0;
        if (x >= m_n) return 1.0;
        double sum = 0.0;
        int limit = std::floor(x);
        for (int i = 0; i <= limit; ++i) sum += pdf(i);
        return sum;
    }
    double mean() const override { return m_n * m_p; }
    double variance() const override { return m_n * m_p * (1.0 - m_p); }
    double xMin() const override { return -0.5; }
    double xMax() const override { return m_n + 0.5; }
    bool isDiscrete() const override { return true; }
};

class GeometricDistribution : public Distribution {
private:
    double m_p;
public:
    GeometricDistribution(double p) : m_p(p) {
        if (m_p <= 0.0 || m_p > 1.0) throw std::invalid_argument("p debe estar en (0, 1]");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k < 1) return 0.0;
        return std::pow(1.0 - m_p, k - 1) * m_p;
    }
    double cdf(double x) const override {
        if (x < 1.0) return 0.0;
        return 1.0 - std::pow(1.0 - m_p, std::floor(x));
    }
    double mean() const override { return 1.0 / m_p; }
    double variance() const override { return (1.0 - m_p) / (m_p * m_p); }
    double xMin() const override { return 0.5; }
    double xMax() const override { return std::ceil(4.0 / m_p); }
    bool isDiscrete() const override { return true; }
};

class NegativeBinomialDistribution : public Distribution {
private:
    int m_r; double m_p;
public:
    NegativeBinomialDistribution(int r, double p) : m_r(r), m_p(p) {
        if (m_r <= 0) throw std::invalid_argument("r debe ser mayor a 0");
        if (m_p <= 0.0 || m_p > 1.0) throw std::invalid_argument("p debe estar en (0, 1]");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k < m_r) return 0.0;
        long double comb = mathFactorial(k - 1) / (mathFactorial(m_r - 1) * mathFactorial(k - m_r));
        return comb * std::pow(m_p, m_r) * std::pow(1.0 - m_p, k - m_r);
    }
    double cdf(double x) const override {
        if (x < m_r) return 0.0;
        double sum = 0.0;
        int limit = std::floor(x);
        for (int i = m_r; i <= limit; ++i) sum += pdf(i);
        return sum;
    }
    double mean() const override { return m_r / m_p; }
    double variance() const override { return (m_r * (1.0 - m_p)) / (m_p * m_p); }
    double xMin() const override { return m_r - 0.5; }
    double xMax() const override { return m_r + std::ceil(4.0 * (1.0 - m_p) / m_p) + 0.5; }
    bool isDiscrete() const override { return true; }
};

class PoissonDistribution : public Distribution {
private:
    double m_lambda;
public:
    PoissonDistribution(double lambda) : m_lambda(lambda) {
        if (m_lambda <= 0.0) throw std::invalid_argument("lambda debe ser mayor a 0");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k < 0) return 0.0;
        return (std::pow(m_lambda, k) * std::exp(-m_lambda)) / mathFactorial(k);
    }
    double cdf(double x) const override {
        if (x < 0.0) return 0.0;
        double sum = 0.0;
        int limit = std::floor(x);
        for (int i = 0; i <= limit; ++i) sum += pdf(i);
        return sum;
    }
    double mean() const override { return m_lambda; }
    double variance() const override { return m_lambda; }
    double xMin() const override { return -0.5; }
    double xMax() const override { return m_lambda + 4.0 * std::sqrt(m_lambda) + 0.5; }
    bool isDiscrete() const override { return true; }
};

class UniformDiscreteDistribution : public Distribution {
private:
    int m_a, m_b;
public:
    UniformDiscreteDistribution(int a, int b) : m_a(a), m_b(b) {
        if (m_a > m_b) throw std::invalid_argument("a debe ser menor o igual a b");
    }
    double pdf(double x) const override {
        int k = std::round(x);
        if (k < m_a || k > m_b) return 0.0;
        return 1.0 / (m_b - m_a + 1);
    }
    double cdf(double x) const override {
        if (x < m_a) return 0.0;
        if (x >= m_b) return 1.0;
        return (std::floor(x) - m_a + 1.0) / (m_b - m_a + 1.0);
    }
    double mean() const override { return (m_a + m_b) / 2.0; }
    double variance() const override { return (std::pow(m_b - m_a + 1, 2) - 1.0) / 12.0; }
    double xMin() const override { return m_a - 0.5; }
    double xMax() const override { return m_b + 0.5; }
    bool isDiscrete() const override { return true; }
};

// DISTRIBUCIONES CONTINUAS

class UniformContinuousDistribution : public Distribution {
private:
    double m_a, m_b;
public:
    UniformContinuousDistribution(double a, double b) : m_a(a), m_b(b) {
        if (m_a >= m_b) throw std::invalid_argument("a debe ser estrictamente menor que b");
    }
    double pdf(double x) const override {
        if (x < m_a || x > m_b) return 0.0;
        return 1.0 / (m_b - m_a);
    }
    double cdf(double x) const override {
        if (x < m_a) return 0.0;
        if (x > m_b) return 1.0;
        return (x - m_a) / (m_b - m_a);
    }
    double mean() const override { return (m_a + m_b) / 2.0; }
    double variance() const override { return std::pow(m_b - m_a, 2) / 12.0; }
    double xMin() const override { return m_a - (m_b - m_a) * 0.1; }
    double xMax() const override { return m_b + (m_b - m_a) * 0.1; }
    bool isDiscrete() const override { return false; }
};

class NormalDistribution : public Distribution {
private:
    double m_mu, m_sigma;
public:
    NormalDistribution(double mu, double sigma) : m_mu(mu), m_sigma(sigma) {
        if (m_sigma <= 0.0) throw std::invalid_argument("sigma debe ser mayor a 0");
    }
    double pdf(double x) const override {
        double exponent = -std::pow(x - m_mu, 2) / (2.0 * std::pow(m_sigma, 2));
        return (1.0 / (m_sigma * std::sqrt(2.0 * M_PI))) * std::exp(exponent);
    }
    double cdf(double x) const override {
       return 0.5 * (1.0 + std::erf((x - m_mu) / (m_sigma * std::sqrt(2.0))));
    }
    double mean() const override { return m_mu; }
    double variance() const override { return m_sigma * m_sigma; }
    double xMin() const override { return m_mu - 4.0 * m_sigma; }
    double xMax() const override { return m_mu + 4.0 * m_sigma; }
    bool isDiscrete() const override { return false; }
};

class ExponentialDistribution : public Distribution {
private:
    double m_lambda;
public:
    ExponentialDistribution(double lambda) : m_lambda(lambda) {
        if (m_lambda <= 0.0) throw std::invalid_argument("lambda debe ser mayor a 0");
    }
    double pdf(double x) const override {
        if (x < 0.0) return 0.0;
        return m_lambda * std::exp(-m_lambda * x);
    }
    double cdf(double x) const override {
        if (x < 0.0) return 0.0;
        return 1.0 - std::exp(-m_lambda * x);
    }
    double mean() const override { return 1.0 / m_lambda; }
    double variance() const override { return 1.0 / (m_lambda * m_lambda); }
    double xMin() const override { return -0.5; }
    double xMax() const override { return 5.0 / m_lambda; }
    bool isDiscrete() const override { return false; }
};

class GammaDistribution : public Distribution {
private:
    double m_alpha, m_beta;
public:
    GammaDistribution(double alpha, double beta) : m_alpha(alpha), m_beta(beta) {
        if (m_alpha <= 0.0 || m_beta <= 0.0) throw std::invalid_argument("Parámetros deben ser mayores a 0");
    }
    double pdf(double x) const override {
        if (x <= 0.0) return 0.0;
        return (std::pow(m_beta, m_alpha) * std::pow(x, m_alpha - 1.0) * std::exp(-m_beta * x)) / tgamma_approx(m_alpha);
    }
    double cdf(double x) const override {
        if (x <= 0.0) return 0.0;
        return gammp_approx(m_alpha, m_beta * x);
    }
    double mean() const override { return m_alpha / m_beta; }
    double variance() const override { return m_alpha / (m_beta * m_beta); }
    double xMin() const override { return -0.5; }
    double xMax() const override {
        double m = mean();
        double s = std::sqrt(variance());
        return m + 5.0 * s;
    }
    bool isDiscrete() const override { return false; }
};

class ChiSquaredDistribution : public Distribution {
private:
    int m_k;
public:
    ChiSquaredDistribution(int k) : m_k(k) {
        if (m_k <= 0) throw std::invalid_argument("Grados de libertad deben ser mayores a 0");
    }
    double pdf(double x) const override {
        if (x <= 0.0) return 0.0;
        double alpha = m_k / 2.0;
        double beta = 0.5;
        return (std::pow(beta, alpha) * std::pow(x, alpha - 1.0) * std::exp(-beta * x)) / tgamma_approx(alpha);
    }
    double cdf(double x) const override {
        if (x <= 0.0) return 0.0;
        return gammp_approx(m_k / 2.0, 0.5 * x);
    }
    double mean() const override { return static_cast<double>(m_k); }
    double variance() const override { return 2.0 * m_k; }
    double xMin() const override { return -0.5; }
    double xMax() const override { return m_k + 5.0 * std::sqrt(2.0 * m_k); }
    bool isDiscrete() const override { return false; }
};

#endif