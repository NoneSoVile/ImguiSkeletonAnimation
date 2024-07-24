//
// Created by leejb on 2020/4/15.
//

#ifndef METALBOX_UTILITIES_H
#define METALBOX_UTILITIES_H

#include <iostream>
#include <string>
#include <sstream>
#include "NvMath.h"
using std::string;
using MB::vec3f;

double getCurrentTimeSeconds();
float clamp(const float& a, const float b, const float c);
float clampEulerAngle_minusPIToPI(const float& a);
vec3f clampEulerAngle_minusPIToPI(const vec3f& a);
float clampEulerAngle_ZeroTo2PI(const float& a);
vec3f clampEulerAngle_ZeroTo2PI(const vec3f& a);

float scalarTriple(vec3f a, vec3f b, vec3f c);
void swap(float& a, float& b);
void getViewFrame(vec3f eye, vec3f at, vec3f up,
        vec3f& x, vec3f& y, vec3f& z);
#define PI (3.1415926535898f)


#endif //METALBOX_UTILITIES_H
