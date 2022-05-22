#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <cmath>

inline double map(double x, double a, double b, double c, double d) {
    return (x - a) * (d - c) / (b - a) + c;
}

inline double lerp(double x, double a, double b) {
    return x + (a - x) * b;
}

// Gerar um inteiro aleatório num intervalo [lower, upper]
inline int randi(int lower, int upper) {
    return map(rand(), 0.0, RAND_MAX, lower, upper);
}

#endif
