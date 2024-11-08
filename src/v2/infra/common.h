#ifndef INFRA_COMMON_H
#define INFRA_COMMON_H

#include <iostream>
#include <cmath>
#include <random>
// #include <algorithm>
// #include <execution>
#include <chrono>

namespace krt {

constexpr double pi = 3.1415926535897932385;

constexpr double infinity = std::numeric_limits<double>::infinity();

constexpr double deg2Rad(const double degree) {
    return degree / 180 * pi;
}

constexpr double rad2Deg(const double radiant) {
    return radiant / pi * 180;
}

inline double randomDouble() {
    static std::uniform_real_distribution<double> uniDist(0.0, 0.9999999999);
    static std::mt19937_64 generator;
    return uniDist(generator);
}

inline double randomDouble(double min, double max) {
    return min + (max - min) * randomDouble();
}

template <typename T>
inline T normalize(const T& data) {return (data + 1) / 2;}

}

#endif