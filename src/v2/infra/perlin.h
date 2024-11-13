#ifndef INFRA_PERLIN_H
#define INFRA_PERLIN_H

#include "vec3.h"

namespace krt {

class Perlin {
    const static size_t pointCount {256};
    Vec3 randVec[pointCount];
    int perms[3][pointCount];
public:
    Perlin() {
        for (size_t i {0}; i < pointCount; ++i) {
            randVec[i] = Vec3::genRandomVec(-1, 1).unitVec();
            for (size_t j {0}; j < 3; ++j) {
                perms[j][i] = i;
            }
        }
        for (size_t row {0}; row < 3; ++row) {
            permutePerm(row);
        }
    }

    double noise(const Point& p) const {
        int integers[3] {static_cast<int>(std::floor(p[0])), static_cast<int>(std::floor(p[1])), static_cast<int>(std::floor(p[2]))};
        double decimals[3] {p[0] - integers[0], p[1] - integers[1], p[2] - integers[2]};
        Vec3 c[2][2][2];
        const static int pointCountMinus1 {pointCount - 1};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 2; ++k) {
                    c[i][j][k] = randVec[
                        perms[0][(integers[0] + i) & pointCountMinus1] ^
                        perms[1][(integers[1] + j) & pointCountMinus1] ^
                        perms[2][(integers[2] + k) & pointCountMinus1]
                    ];
                }
            }
        }
        return interpolate(c, decimals);
    }

    double turbulence(const Point& p, int depth) const {
        double acc {0};
        double weight {1};
        Point tp {p};
        for (int i {0}; i < depth; ++i, weight *= 0.5, tp *= 2) {
            acc += weight * noise(tp);
        }
        return std::abs(acc);
    }

private:
    void permutePerm(size_t row) {
        for (size_t i {pointCount - 1}; i > 0; --i) {
            int toSwap {randomInt(0, i)};
            std::swap(perms[row][toSwap], perms[row][i]);
        }
    }

    static double interpolate(const Vec3 c[2][2][2], double uvw[3]) {
        double uuvvww[3];
        for (int i {0}; i < 3; ++i) {
            uuvvww[i] = uvw[i] * uvw[i] * (3 - 2 * uvw[i]);
        }
        double acc {0.0};
        for (int i {0}; i < 2; ++i) {
            for (int j {0}; j < 2; ++j) {
                for (int k {0}; k < 2; ++k) {
                    Vec3 weight {uvw[0] - i, uvw[1] - j, uvw[2] - k};
                    acc += (i * uuvvww[0] + (1 - i) * (1 - uuvvww[0]))
                        * (j * uuvvww[1] + (1 - j) * (1 - uuvvww[1]))
                        * (k * uuvvww[2] + (1 - k) * (1 - uuvvww[2]))
                        * dot(c[i][j][k], weight);
                }
            }
        }
        return acc;
    }
};

}

#endif