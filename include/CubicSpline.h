#pragma once
#include <vector>
#include "Vector.h"
using MB::vec2f;
using MB::vec3f;

class CubicSpline {
private:
    std::vector<double> x; // nodes x0, x1, ..., xn
    std::vector<double> y; // function values f(x0), f(x1), ..., f(xn)
    std::vector<double> h; // subinterval lengths h0, h1, ..., hn-1
    std::vector<double> a; // coefficients of cubic polynomials
    std::vector<double> b;
    std::vector<double> c;
    std::vector<double> d;

public:
    CubicSpline(const std::vector<double>& nodes, const std::vector<double>& values) {
        x = nodes;
        y = values;
        int n = x.size() - 1;
        h.resize(n);
        a.resize(n + 1);
        b.resize(n + 1);
        c.resize(n + 1);
        d.resize(n + 1);
        
        // Step 1: Compute subinterval lengths
        for (int i = 0; i < n; i++) {
            h[i] = x[i + 1] - x[i];
        }
        
        // Step 2: Solve tridiagonal system of equations
        std::vector<double> alpha(n);
         std::vector<double> beta(n);
        alpha[0] = 0;
        beta[0] = 0;
        
        for (int i = 1; i < n; i++) {
            double hi = h[i - 1];
            double hi1 = h[i];
            double fi = (y[i + 1] - y[i]) / hi1 - (y[i] - y[i - 1]) / hi;
            alpha[i] = hi / (hi + hi1);
            beta[i] = 3 * (fi / (hi + hi1) - (fi / hi));
        }
        
        std::vector<double> c_prime(n + 1);
        c_prime[0] = 0;
        c_prime[n] = 0;
        
        for (int i = 1; i < n; i++) {
            double hi = h[i - 1];
            double hi1 = h[i];
            double alpha_prime = alpha[i] - alpha[i] * alpha[i - 1];
            double beta_prime = beta[i] - alpha[i] * beta[i - 1];
            c_prime[i] = beta_prime / (alpha_prime - alpha_prime * c_prime[i - 1]);
        }
        
        for (int i = n - 1; i > 0; i--) {
            c[i] = c_prime[i] - alpha[i] * c[i + 1];
        }
        
        for (int i = 0; i < n; i++) {
            double hi = h[i];
            double ci = c[i];
            double ci1 = c[i + 1];
            double yi = y[i];
            double yi1 = y[i + 1];
            a[i] = (ci1 - ci) / (3 * hi);
            b[i] = (yi1 - yi) / hi - (hi / 3) * (2 * ci + ci1);
            d[i] = yi;
        }
    }
    
    double interpolate(double x_val) {
        int n = x.size() - 1;
        
        // Find the subinterval [xj, xj+1] that contains x_val
        int j = 0;
        while (j < n && x_val > x[j + 1]) {
            j++;
        }
        
        // Evaluate the cubic spline polynomial Sj(x) at x_val
        double xi = x[j];
        double hi = h[j];
        double ai = a[j];
        double bi = b[j];
        double ci = c[j];
        double di = d[j];
        double t = (x_val - xi) / hi;
        double t2 = t * t;
        double t3 = t * t2;
        double result = di + bi * t + ci * t2 + ai * t3;
        
        return result;
    }

    std::vector<vec3f> interpolateAllPoints(int numPoints) {
        std::vector<vec3f> interpolatedPoints;
        int extra_Points = 0;// numPoints / 2;
        double stepSize = 1.0f / (numPoints - 1);
        float offset = -extra_Points / 2.0 * stepSize; //t can go through out of interval[0, 1]
        int totalPoints = numPoints + extra_Points;
        for (int i = 0; i < totalPoints; i++) {
            double t = i * stepSize + offset;
            
            vec3f newPt = interpolate(t);
            interpolatedPoints.push_back(newPt);
        }

        return interpolatedPoints;
    }

};