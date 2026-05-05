#ifndef PROBABILITY_H
#define PROBABILITY_H

#include <stdexcept>
#include <functional>
#include <cmath>
typedef long double ld;

class Probability {
public:

    static ld classic(ld fav_cases, ld total) {
        if (total == 0) throw std::invalid_argument("total no puede ser 0.");
        return fav_cases / total;
    }

    static ld conditional(ld inter, ld b) {
        if (b == 0) throw std::invalid_argument("B no puede ser 0.");
        return inter / b;
    }


    static ld integrate(std::function<ld(ld)> f, ld a, ld b, int n = 1000) {
        if (n % 2 != 0) n++;
        ld h = (b - a) / n;
        ld sum = f(a) + f(b);
        for (int i = 1; i < n; i++) {
            ld x = a + i * h;
            sum += (i % 2 == 0) ? 2 * f(x) : 4 * f(x);
        }
        return (h / 3.0) * sum;
    }

    static bool isDenFunc(std::function<ld(ld)> f, ld a, ld b) {
        for(int i = 0; i <= 100; ++i) {
            if(f(a + (b-a) * i / 100.0) < 0) return false;
        }
        ld area = integrate(f, a, b);
        return std::abs(area - 1.0) < 1e-3;
    }

    static ld expectation(std::function<ld(ld)> f, ld a, ld b) {
        return integrate([&](ld x){ return x * f(x); }, a, b);
    }

    // n-momento
    static ld moment(std::function<ld(ld)> f, ld a, ld b, int n) {
        return integrate([&](ld x){ return std::pow(x, n) * f(x); }, a, b);
    }

    // Varianza
    static ld variance(std::function<ld(ld)> f, ld a, ld b) {
        ld ex = expectation(f, a, b);
        ld ex2 = moment(f, a, b, 2);
        return ex2 - (ex * ex);
    }
};

#endif