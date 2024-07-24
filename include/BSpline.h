#include <iostream>
#include <vector>
#include "Vector.h"
using MB::vec3f;
class BSpline {
public:
  BSpline() {}

  std::vector<vec3f> generatePoints(int numPoints, int degree, std::vector<vec3f> controlPoints) {
      std::vector<vec3f> points;
      double tStep = 1.0 / (numPoints - 1);

      for (int i = 0; i < numPoints; i++) {
          double t = i * tStep;
          vec3f point;
          interpolate(t, degree, controlPoints, point);
          points.push_back(point);
      }

      return points;
  }

  void interpolate(double t, int degree, std::vector<vec3f> controlPoints,  vec3f &result)
  {
      std::vector<double> knots;
      std::vector<double> weights;
      int i, j, s, l;        // function-scoped iteration variables
      int n = controlPoints.size(); // controlPoints count
      int d = 3;             // point dimensionality

      if (degree < 1)
          throw std::runtime_error("degree must be at least 1 (linear)");
      if (degree > (n - 1))
          throw std::runtime_error("degree must be less than or equal to point count - 1");

      if (weights.empty())
      {
          // build weight vector of length [n]
          weights.resize(n, 1);
      }

      if (knots.empty())
      {
          // build knot vector of length [n + degree + 1]
          knots.resize(n + degree + 1);
          for (i = 0; i < n + degree + 1; i++)
          {
              knots[i] = i;
          }
      }
      else
      {
          if (knots.size() != n + degree + 1)
              throw std::runtime_error("bad knot vector length");
      }

      std::vector<int> domain = {
          degree,
          (int)knots.size() - 1 - degree};

      // remap t to the domain where the spline is defined
      double low = knots
          [domain[0]];
      double high = knots[domain[1]];
      t = t * (high - low) + low;

      if (t < low || t > high)
          throw std::runtime_error("out of bounds");

      // find s (the spline segment) for the [t] value provided
      for (s = domain[0]; s < domain[1]; s++)
      {
          if (t >= knots[s] && t <= knots[s + 1])
          {
              break;
          }
      }

      // convert controlPoints to homogeneous coordinates
      std::vector<std::vector<double>> v(n, std::vector<double>(d + 1));
      for (i = 0; i < n; i++)
      {
          v[i][0] = controlPoints[i].x * weights[i];
          v[i][1] = controlPoints[i].y * weights[i];
          v[i][2] = controlPoints[i].z * weights[i];
          v[i][d] = weights[i];
      }

      // l (level) goes from 1 to the curve degree + 1
      double alpha;
      for (l = 1; l <= degree + 1; l++)
      {
          // build level l of the pyramid
          for (i = s; i > s - degree - 1 + l; i--)
          {
              alpha = (t - knots[i]) / (knots[i + degree + 1 - l] - knots[i]);

              // interpolate each component
              for (j = 0; j < d + 1; j++)
              {
                  v[i][j] = (1 - alpha) * v[i - 1][j] + alpha * v[i][j];
              }
          }
      }

      // convert back
      // to cartesian and return
      result.x = v[s][0] / v[s][d];
      result.y = v[s][1] / v[s][d];
      result.z = v[s][2] / v[s][d];
  }
};