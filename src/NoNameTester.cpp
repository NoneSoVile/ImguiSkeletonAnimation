#include "NoNameTester.h"
#include <opencv2/opencv.hpp>
using namespace cv;
#include <vector>
#include <vector>
#include <cmath>
#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using std::string;
#define VIDEO_WIDTH 1280
#define VIDEO_HEIGHT 960
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

int w = VIDEO_WIDTH;
int h = VIDEO_HEIGHT;
#define M_PI 3.141592
Mat cropFisheyeToRect(const Mat& fisheyeImage, int rectWidth, int rectHeight)
{
	int fisheyeWidth = fisheyeImage.cols;
	int fisheyeHeight = fisheyeImage.rows;
	int fisheyeCenterX = fisheyeWidth / 2;
	int fisheyeCenterY = fisheyeHeight / 2;

	// 计算椭圆的参数
	int ellipseWidth = fisheyeWidth * 0.6;  // 椭圆的宽度
	int ellipseHeight = fisheyeHeight * 0.6;  // 椭圆的高度
	int ellipseCenterX = fisheyeCenterX;  // 椭圆的中心 x 坐标
	int ellipseCenterY = fisheyeCenterY;  // 椭圆的中心 y 坐标

	// 创建矩形图像
	Mat rectImage(rectHeight, rectWidth, CV_8UC3);

	for (int y = 0; y < rectHeight; y++)
	{
		for (int x = 0; x < rectWidth; x++)
		{
			// 将矩形图像坐标映射回鱼眼图像坐标
			double xMap = x / (double)rectWidth * ellipseWidth - ellipseWidth / 2.0 + ellipseCenterX;
			double yMap = y / (double)rectHeight * ellipseHeight - ellipseHeight / 2.0 + ellipseCenterY;

			// 检查坐标是否在鱼眼图像范围内
			if (xMap >= 0 && xMap < fisheyeWidth && yMap >= 0 && yMap < fisheyeHeight)
			{
				// 将鱼眼图像中的像素值赋给矩形图像
				rectImage.at<Vec3b>(y, x) = fisheyeImage.at<Vec3b>(yMap, xMap);
			}
			else
			{
				// 超出鱼眼图像范围的像素点置为黑色
				rectImage.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
			}
		}
	}

	return rectImage;
}
Mat fisheyeToWideAngle(const Mat& fisheyeImage, int wideAngleWidth, int wideAngleHeight, double wideAngleFOV)
{
	int fisheyeWidth = fisheyeImage.cols;
	int fisheyeHeight = fisheyeImage.rows;
	double fisheyeAspectRatio = fisheyeWidth / (double)fisheyeHeight;
	double fisheyeCenterX = fisheyeWidth / 2.0;
	double fisheyeCenterY = fisheyeHeight / 2.0;

	double wideAngleAspectRatio = wideAngleWidth / (double)wideAngleHeight;
	double wideAngleCenterX = wideAngleWidth / 2.0;
	double wideAngleCenterY = wideAngleHeight / 2.0;

	// 计算超广角图像的焦距
	double wideAngleFocalLength = (wideAngleWidth / 2.0) / tan(wideAngleFOV / 2.0);

	Mat wideAngleImage(wideAngleHeight, wideAngleWidth, CV_8UC3);

	for (int y = 0; y < wideAngleHeight; y++)
	{
		for (int x = 0; x < wideAngleWidth; x++)
		{
			double theta = atan2(y - wideAngleCenterY, x - wideAngleCenterX);
			double r = (wideAngleAspectRatio * wideAngleFocalLength) / sqrt(pow(x - wideAngleCenterX, 2) + pow(y - wideAngleCenterY, 2));

			int fisheyeX = (int)(r * cos(theta) + fisheyeCenterX);
			int fisheyeY = (int)(r * sin(theta) + fisheyeCenterY);

			if (fisheyeX >= 0 && fisheyeX < fisheyeWidth && fisheyeY >= 0 && fisheyeY < fisheyeHeight)
			{
				wideAngleImage.at<Vec3b>(y, x) = fisheyeImage.at<Vec3b>(fisheyeY, fisheyeX);
			}
			else
			{
				wideAngleImage.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
			}
		}
	}

	return wideAngleImage;
}

void getEllipsePoints(const cv::RotatedRect& ellipseROI, std::vector<cv::Point2f>& points, int numPoints) {
	points.clear();

	float angleStep = 360.0f / numPoints;

	for (float angle = 0.0f; angle < 360.0f; angle += angleStep) {
		cv::Point2f point;
		float radian = angle * CV_PI / 180.0f;
		point.x = ellipseROI.center.x + 0.5f * ellipseROI.size.width * std::cos(radian);
		point.y = ellipseROI.center.y + 0.5f * ellipseROI.size.height * std::sin(radian);
		points.push_back(point);
	}
}

cv::Mat transformEllipseToRectangle(const cv::Mat& inputImage, const cv::Rect& rectangleROI, int numPoints) {
	// Calculate the aspect ratio of the rectangle ROI
	float aspectRatio = static_cast<float>(rectangleROI.width) / rectangleROI.height;

	// Create an ellipse ROI with the same center as the rectangle and adjusted size to match the aspect ratio
	cv::Size ellipseSize(rectangleROI.height * aspectRatio, rectangleROI.height);
	cv::Point2f ellipseCenter(rectangleROI.x + rectangleROI.width / 2.0f, rectangleROI.y + rectangleROI.height / 2.0f);
	cv::RotatedRect ellipseROI(ellipseCenter, ellipseSize, 0);

	// Get points on the ellipse
	std::vector<cv::Point2f> ellipsePoints;
	getEllipsePoints(ellipseROI, ellipsePoints, numPoints);

	// Define the destination points for the perspective transformation
	std::vector<cv::Point2f> rectanglePoints;
	float angleStep = CV_2PI / (numPoints);

	for (int i = 0; i < numPoints; ++i) {
		// Calculate the angle based on the index
		float angle = i * angleStep;

		// Map the angle to the corresponding point on the ellipse
		float ellipseX = ellipseROI.center.x + (ellipseROI.size.width / 2.0f) * cos(angle);
		float ellipseY = ellipseROI.center.y + (ellipseROI.size.height / 2.0f) * sin(angle);

		// Map the ellipse point to the corresponding point on the rectangle
		float x = rectangleROI.x + (ellipseX - ellipseROI.center.x) * (rectangleROI.width / ellipseROI.size.width);
		float y = rectangleROI.y + (ellipseY - ellipseROI.center.y) * (rectangleROI.height / ellipseROI.size.height);

		rectanglePoints.push_back(cv::Point2f(x, y));
	}




	// Create a transformed output image with the size of the rectangle
	cv::Mat transformedImage(rectangleROI.size(), inputImage.type(), cv::Scalar(0));

	// Warp the input image using the perspective transform
	cv::Mat perspectiveMatrix = cv::getPerspectiveTransform(ellipsePoints.data(), rectanglePoints.data());
	cv::warpPerspective(inputImage, transformedImage, perspectiveMatrix, transformedImage.size());

	return transformedImage;
}

int mainTest() {
	// Load the input image
	cv::Mat inputImage = cv::imread("resource/left.jpg");

	// Set the number of points on the ellipse
	int numPoints = 64;
	// Define the rectangle ROI as the size of the input image
	cv::Rect rectangleROI(0, 0, inputImage.cols, inputImage.rows);
	// Map the ellipse to the rectangle
	cv::Mat transformedImage = transformEllipseToRectangle(inputImage, rectangleROI, numPoints);

	// Display the transformed image
	cv::imshow("Transformed Image", transformedImage);
	cv::waitKey(0);

	return 0;
}




#include <cmath>

// Vertex shader source code
const char* vertexShaderSource =
"#version 300 es\n"
"layout (location = 0) in vec2 position;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(position, 0.0, 1.0);\n"
"}\n";

// Fragment shader source code
const char* fragmentShaderSource =
"#version 300 es\n"
"precision mediump float;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";
// Specify the number of subdivisions for the rounded corners
const int numSubdivisions = 30;
void initialize()
{
	// Create and compile the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Create and link the shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	// Specify the radius for rounded corners
	//float radius = 0.1f;

	// Create a vertex buffer object (VBO) to store the vertex data
	GLuint vbo;
	glGenBuffers(1, &vbo);

	// Specify the vertices of the rectangle with round corners
	/*float vertices[360 * 2];
	for (int i = 0; i < 360; ++i)
	{
		float angle = i * M_PI / 180.0f;
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		vertices[i * 2] = x;
		vertices[i * 2 + 1] = y;
	}*/



	// Specify the vertices of the rectangle with round corners
/*	float vertices[] = {
	// Top left corner
	-0.5f, 0.5f,                 // Vertex 0
	-0.5f + radius, 0.5f,        // Vertex 1
	-0.5f, 0.5f - radius,        // Vertex 2
	// Top right corner
	0.5f - radius, 0.5f,         // Vertex 3
	0.5f, 0.5f,                  // Vertex 4
	0.5f, 0.5f - radius,         // Vertex 5
	// Bottom right corner
	0.5f, -0.5f + radius,        // Vertex 6
	0.5f, -0.5f,                 // Vertex 7
	0.5f - radius, -0.5f,        // Vertex 8
	// Bottom left corner
	-0.5f + radius, -0.5f,       // Vertex 9
	-0.5f, -0.5f,                // Vertex 10
	-0.5f, -0.5f + radius         // Vertex 11
};*/




// Calculate the angle increment for each subdivision
	float angleIncrement = (2.0f * M_PI) / numSubdivisions;

	// Specify the radius for rounded corners
	float radius = 0.1f;

	// Specify the vertices of the rectangle with round corners
	float vertices[(7 + numSubdivisions) * 2];

	// Specify the vertices for the rectangle body
	vertices[0] = -0.5f;
	vertices[1] = -0.5f + radius;
	vertices[2] = -0.5f;
	vertices[3] = 0.5f - radius;
	vertices[4] = -0.5f + radius;
	vertices[5] = 0.5f;
	vertices[6] = 0.5f - radius;
	vertices[7] = 0.5f;
	vertices[8] = 0.5f;
	vertices[9] = 0.5f - radius;
	vertices[10] = 0.5f;
	vertices[11] = -0.5f + radius;
	vertices[12] = -0.5f;
	vertices[13] = -0.5f;

	// Specify the vertices for the rounded corners
	for (int i = 0; i < numSubdivisions; ++i) {
		float angle = i * angleIncrement;
		vertices[14 + i * 2] = -0.5f + radius * cos(angle);
		vertices[15 + i * 2] = -0.5f + radius * sin(angle);
	}

	// Bind the VBO and copy the vertex data to the GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Create a vertex array object (VAO) to store the vertex attribute configuration
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Specify the vertex attribute configuration
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);


}

void draw()
{
	// Set up the viewport
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// Draw the rounded rectangle using triangle fan
	glDrawArrays(GL_TRIANGLE_FAN, 0, 7 + numSubdivisions);

	// Swap buffers and handle events
}
#include <iostream>
#include <regex>
#include <string>
std::string replaceNumericValuesxxx(const std::string& input, const std::string& pattern, int specificDigit) {
	std::regex regex(pattern);
	std::string replacedString = std::regex_replace(input, regex, "$1" + std::to_string(specificDigit) + "$2");

	return replacedString;
}

std::string replaceNumericValues(const std::string& input, const std::string& pattern, int specificDigit) {
	std::regex regex(pattern);
	string replacestr = "$1 " + std::to_string(specificDigit) + "$2 " + std::to_string(specificDigit) + "$3";
	std::string replacedString = std::regex_replace(input, regex, replacestr);

	return replacedString;
}