#ifndef PROBABILITY_H
#define PROBABILITY_H

#include <stdexcept>
#include <functional>
#include <string>
#include <cmath>
#include <vector>

typedef long double ld;

enum State {
    INIT,
    INT_DIG,
    DEC_P,
    DEC_DIG,
    VAR_X,
    OPERATOR,
    POW,
    EXP,
    ERROR
};

// Form ax^n
struct Linear {
    double a;
    int n;
};

class Probability {
public:

    std::vector<Linear> validate_exp(std::string &candidate) {
        State current = INIT;
        std::vector<Linear> linear_info;

        Linear curr = {0.0, 0};

        std::string num_buffer = "";
        double signo_actual = 1.0;

        for (char c : candidate) {
            if (isspace(c)) continue;

            switch (current) {
            case INIT:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = INT_DIG;
                }
                else if (c == 'x' || c == 'X') {
                    curr.a = signo_actual;
                    curr.n = 1;
                    current = VAR_X;
                }
                else if (c == '+' || c == '-') {
                    signo_actual = (c == '-') ? -1.0 : 1.0;
                    current = OPERATOR;
                }
                else current = ERROR;
                break;

            case INT_DIG:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = INT_DIG;
                }
                else if (c == '.') {
                    num_buffer += c;
                    current = DEC_P;
                }
                else if (c == 'x' || c == 'X') {
                    curr.a = std::stod(num_buffer) * signo_actual;
                    num_buffer = "";
                    curr.n = 1;
                    current = VAR_X;
                }
                else if (c == '+' || c == '-') {
                    curr.a = std::stod(num_buffer) * signo_actual;
                    curr.n = 0;
                    linear_info.push_back(curr);

                    curr = {0.0, 0};
                    signo_actual = (c == '-') ? -1.0 : 1.0;
                    num_buffer = "";
                    current = OPERATOR;
                }
                else current = ERROR;
                break;

            case DEC_P:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = DEC_DIG;
                }
                else current = ERROR;
                break;

            case DEC_DIG:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = DEC_DIG;
                }
                else if (c == 'x' || c == 'X') {
                    curr.a = std::stod(num_buffer) * signo_actual;
                    num_buffer = "";
                    curr.n = 1;
                    current = VAR_X;
                }
                else if (c == '+' || c == '-') {
                    curr.a = std::stod(num_buffer) * signo_actual;
                    curr.n = 0;
                    linear_info.push_back(curr);

                    curr = {0.0, 0};
                    signo_actual = (c == '-') ? -1.0 : 1.0;
                    num_buffer = "";
                    current = OPERATOR;
                }
                else current = ERROR;
                break;

            case VAR_X:
                if (c == '+' || c == '-') {
                    linear_info.push_back(curr);

                    curr = {0.0, 0};
                    signo_actual = (c == '-') ? -1.0 : 1.0;
                    current = OPERATOR;
                }
                else if (c == '^') {
                    current = POW;
                }
                else current = ERROR;
                break;

            case OPERATOR:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = INT_DIG;
                }
                else if (c == 'x' || c == 'X') {
                    curr.n = 1;
                    curr.a = signo_actual;
                    current = VAR_X;
                }
                else current = ERROR;
                break;

            case POW:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = EXP;
                }
                else current = ERROR;
                break;

            case EXP:
                if (isdigit(c)) {
                    num_buffer += c;
                    current = EXP;
                }
                else if (c == '+' || c == '-') {
                    curr.n = std::stoi(num_buffer);
                    linear_info.push_back(curr);

                    curr = {0.0, 0};
                    signo_actual = (c == '-') ? -1.0 : 1.0;
                    num_buffer = "";
                    current = OPERATOR;
                }
                else current = ERROR;
                break;

            case ERROR:
                return {};
            }

            if (current == ERROR) return {};
        }

        if (current == INT_DIG || current == DEC_DIG) {
            curr.a = std::stod(num_buffer) * signo_actual;
            curr.n = 0;
            linear_info.push_back(curr);
        }
        else if (current == VAR_X) {
            linear_info.push_back(curr);
        }
        else if (current == EXP) {
            curr.n = std::stoi(num_buffer);
            linear_info.push_back(curr);
        }

        if (current == INT_DIG || current == DEC_DIG || current == VAR_X || current == EXP) {
            return linear_info;
        }

        return {};
    }

    // Proba clasica
    static ld classic(ld fav_cases, ld total) {
        if (total == 0) throw std::invalid_argument("total no puede ser 0.");
        return fav_cases / total;
    }

    // proba condicional
    static ld conditional(ld inter, ld b) {
        if (b == 0) throw std::invalid_argument("B no puede ser 0.");
        return inter / b;
    }

    static ld integrate(const std::vector<Linear>& tokens, ld a, ld b) {
        int n = 1000;
        ld h = (b - a) / n;

        auto evaluate = [&tokens](ld x) -> ld {
            ld sum = 0.0;
            for (const auto& term : tokens) {
                sum += term.a * std::pow(x, term.n);
            }
            return sum;
        };

        ld integral = evaluate(a) + evaluate(b);

        for (int i = 1; i < n; i++) {
            ld x = a + i * h;
            if (i % 2 == 0) {
                integral += 2.0 * evaluate(x);
            } else {
                integral += 4.0 * evaluate(x);
            }
        }

        return (h / 3.0) * integral;
    }

    static bool isDenFunc(const std::vector<Linear>& tokens, ld a, ld b) {

        ld area = integrate(tokens, a, b);
        if (std::abs(area - 1.0) > 1e-4) return false;
        auto evaluate = [&tokens](ld x) -> ld {
            ld sum = 0.0;
            for (const auto& term : tokens) sum += term.a * std::pow(x, term.n);
            return sum;
        };

        for (int i = 0; i <= 100; i++) {
            ld x = a + i * ((b - a) / 100.0);
            if (evaluate(x) < -1e-9) return false;
        }

        return true;
    }

    static ld expectation(const std::vector<Linear>& tokens, ld a, ld b) {
        std::vector<Linear> x_f = tokens;
        for (auto& term : x_f) {
            term.n += 1;
        }
        return integrate(x_f, a, b);
    }

    static ld moment(const std::vector<Linear>& tokens, ld a, ld b, int grado) {
        std::vector<Linear> xn_f = tokens;
        for (auto& term : xn_f) {
            term.n += grado;
        }
        return integrate(xn_f, a, b);
    }

    static ld variance(const std::vector<Linear>& tokens, ld a, ld b) {
        ld ex = expectation(tokens, a, b);
        ld ex2 = moment(tokens, a, b, 2);
        return ex2 - (ex * ex);
    }
};

#endif