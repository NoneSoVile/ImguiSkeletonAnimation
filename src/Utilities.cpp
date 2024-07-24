#include "Utilities.h"
#include <sstream>
#include <opencv2/imgproc/types_c.h>
#include<opencv2/imgproc/imgproc_c.h>
#include <opencv2/opencv.hpp>
#include<windows.h>


cv::Point2f unDistortImg(int i, int j, cv::Mat A, cv::Mat K, double param, int wide_s, int height_s)
{
	cv::Mat AIN;
	cv::Mat cvC = cv::Mat(3, 1, CV_64F);
	cv::Mat camera_c;
	cv::Mat imgc;
	AIN = A.clone();
	double* Avalue = (double*)AIN.data;
	Avalue[0] = Avalue[0] * param;
	Avalue[4] = Avalue[4] * param;

	/*cv::Mat cvC = cv::Mat(3, 1, CV_64F);*/
	Avalue = (double*)cvC.data;
	Avalue[0] = i;
	Avalue[1] = j;
	Avalue[2] = 1;

	camera_c = AIN.inv() * cvC;

	double* p = (double*)camera_c.data;
	p[0] = p[0] / p[2];
	p[1] = p[1] / p[2];

	double lr = sqrt(p[0] * p[0] + p[1] * p[1]);
	double threta = atan(lr);
	double threta2 = threta * threta;
	double threta4 = threta2 * threta2;
	double threta6 = threta4 * threta2;
	double threta8 = threta4 * threta4;

	double* k = (double*)K.data;
	double dtemp;
	dtemp = k[0] * threta2;
	dtemp = dtemp + 1;

	dtemp = dtemp + k[1] * threta4;
	dtemp = dtemp + k[2] * threta6;
	dtemp = dtemp + k[3] * threta8;

	double thrta_d = threta * dtemp;

	if (abs(lr - 0) < 0.000001)
	{
		lr = thrta_d;
	}

	double scale = thrta_d / lr;
	Avalue[0] = scale * p[0];
	Avalue[1] = scale * p[1];
	Avalue[2] = 1;

	imgc = A * cvC;
	p = (double*)imgc.data;

	return cv::Point2f(p[0], p[1]);
}
void OnSpreadImg(cv::Mat sourceMat, cv::Mat A, cv::Mat K, double param, cv::Mat& rMat)
{
	cv::Point2f rPoint;
	int x_w;
	int y_w;
	rMat = cv::Mat::zeros(sourceMat.rows, sourceMat.cols, CV_8UC3);
	for (int j = 0; j < sourceMat.rows; j++)
	{
		for (int i = 0; i < sourceMat.cols; i++)
		{
			rPoint = unDistortImg(i, j, A, K, param, sourceMat.cols, sourceMat.rows);
			x_w = rPoint.x;
			y_w = rPoint.y;

			if ((x_w >= 0) && (x_w < sourceMat.cols) && (y_w >= 0) && (y_w < sourceMat.rows))
			{
				rMat.at<cv::Vec3b>(j, i)[0] = sourceMat.at < cv::Vec3b >(y_w, x_w)[0];
				rMat.at<cv::Vec3b>(j, i)[1] = sourceMat.at < cv::Vec3b >(y_w, x_w)[1];
				rMat.at<cv::Vec3b>(j, i)[2] = sourceMat.at < cv::Vec3b >(y_w, x_w)[2];
			}
		}
	}
}


/*void test()
{
	cv::Mat sourceMat = cv::imread("camera_3.jpg");

	cv::Vec4d K(1.65807120e-01, 1.43721214e-05, 1.98244942e-02, -1.12596307e-02);
	cv::Matx33d A(2.75647579e+02, 0, 6.43639469e+02, 0, 2.75704454e+02, 4.75082798e+02, 0, 0, 1);

	cv::Mat rMat;
	OnSpreadImg(sourceMat, cv::Mat(A), cv::Mat(K), 0.5, rMat);

	cv::imshow("ffr", rMat);
	cv::waitKey(0);
}*/

double getCurrentTimeSeconds()
{
	/* need to recode in windows*/
				// get the tick frequency from the OS
	LARGE_INTEGER temp;
	QueryPerformanceFrequency((LARGE_INTEGER*)&temp);
	// convert to type in which it is needed
	double freq = ((double)temp.QuadPart);

	QueryPerformanceCounter((LARGE_INTEGER*)&temp);
	return
		(((double)temp.QuadPart) / freq);
	return 0.0;
}

float clamp(const float& a, const float b, const float c)
{
	return max(b, min(a, c));
}

float clampEulerAngle_minusPIToPI(const float& a) {
	float angle = a;
	while (angle > PI) {
		angle += -2 * PI;
	}

	while (angle < -PI) {
		angle += 2 * PI;
	}
	return angle;
}

float scalarTriple(vec3f a, vec3f b, vec3f c) {
	vec3f m = a.cross(b);
	float u = dot(m, c);
	return u;
}

void swap(float& a, float& b) {
	float t = a;
	a = b;
	b = t;
}

float clampEulerAngle_ZeroTo2PI(const float& a) {
	float angle = a;
	while (angle > 2 * PI) {
		angle += -2 * PI;
	}

	while (angle < 0) {
		angle += 2 * PI;
	}
	return angle;
}

vec3f clampEulerAngle_ZeroTo2PI(const vec3f& a) {
	vec3f angle = a;
	for (int i = 0; i < 3; ++i) {
		while (angle[i] > 2 * PI) {
			angle[i] += -2 * PI;
		}

		while (angle[i] < 0) {
			angle[i] += 2 * PI;
		}
	}

	return angle;
}

vec3f clampEulerAngle_minusPIToPI(const vec3f& a) {
	vec3f angle = a;
	for (int i = 0; i < 3; ++i) {
		while (angle[i] > PI) {
			angle[i] += -2 * PI;
		}

		while (angle[i] < -PI) {
			angle[i] += 2 * PI;
		}
	}

	return angle;
}

void getViewFrame(vec3f eye, vec3f center, vec3f up,
	vec3f& x, vec3f& y, vec3f& z) {//out x, y, z frame

	// make rotation matrix

	// Z vector
	z.x = eye.x - center.x;
	z.y = eye.y - center.y;
	z.z = eye.z - center.z;
	z = normalize(z);

	// Y vector
	y.x = up.x;
	y.y = up.y;
	y.z = up.z;

	// X vector = Y cross Z
	x = cross(y, z);

	// Recompute Y = Z cross X
	y = cross(z, x);

	// cross product gives area of parallelogram, which is < 1.0 for
	// non-perpendicular unit-length vectors; so normalize x, y here
	x = normalize(x);
	y = normalize(y);
}