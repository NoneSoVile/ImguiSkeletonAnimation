#include <vector>
#include "Vector.h"
using MB::vec2f;
using MB::vec3f;

#include <iostream>
#include <vector>
#include <cmath>

class BezierCurve {
public:

    BezierCurve(const std::vector<vec3f>& controlPoints) : controlPoints(controlPoints) {}

    vec3f calculateBezierPoint(double t) {
        int n = controlPoints.size() - 1;
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;

        for (int i = 0; i <= n; ++i) {
            double coefficient = binomialCoefficient(n, i) * std::pow(t, i) * std::pow(1 - t, n - i);
            x += controlPoints[i].x * coefficient;
            y += controlPoints[i].y * coefficient;
            z += controlPoints[i].z * coefficient;
        }

        return vec3f(x, y, z);
    }

    vec3f calculateDeCasteljauPoint(double t) {
        std::vector<vec3f> points = controlPoints;
        int n = points.size() - 1;

        while (n > 0) {
            for (int i = 0; i < n; ++i) {
                points[i] = (1 - t)
                    * points[i] + t * points[i + 1];
            }
            n--;
        }

        return points[0];
    }


    std::vector<vec3f> interpolateAllPoints(int numPoints)  {
        std::vector<vec3f> interpolatedPoints;
        int extra_Points = 0;// numPoints / 2;
        double stepSize = 1.0f / (numPoints - 1);
        float offset = -extra_Points / 2.0 * stepSize ; //t can go through out of interval[0, 1]
        int totalPoints = numPoints + extra_Points;
        for (int i = 0; i < totalPoints; i++) {
            double t = i * stepSize + offset;
            //vec3f newPt = calculateBezierPoint(t);
            vec3f newPt = calculateDeCasteljauPoint(t);
            interpolatedPoints.push_back(newPt);
        }

        return interpolatedPoints;
    }

private:
    std::vector<vec3f> controlPoints;

    double factorial(int n) {
        double result = 1.0;
        for (int i = 1; i <= n; ++i) {
            result *= i;
        }
        return result;
    }

    double binomialCoefficient(int n, int i) {
        return factorial(n) / (factorial(i) * factorial(n - i));
    }
};