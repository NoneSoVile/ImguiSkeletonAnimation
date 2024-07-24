#pragma once
#ifndef __COMMON_H_
#define __COMMON_H_

#include <memory>
#include <opencv2/opencv.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream>
#include <vector>
#include <stdint.h>

#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#define VENDOR_DATA_PATH ""    
#define TEMP_DATA_PATH ""
#define  AVM_PATH       "/avm/"
#define DATA_PATH   ""
#define LOGI  printf
#define LOGD  printf
#define LOGE  printf
#define PI   3.1415926f

bool is_file_exist(const char* fileName);
double getCurrentTimeSeconds();

int SetupTexture(GLuint& texture, int width, int height);
int SetupTextureData(GLuint& texture, int width, int height, const void* data);
int SetupBufferData(GLuint& buffer, int size, const void* data);
int SetupSSBO(GLuint& buffer, const void* data, int elemSize, int bufSize);
bool isTexture(GLuint textureID);
void _uploadImageToTexture(const cv::Mat& image, GLuint& textureID);
void copyImage(const cv::Mat& inputImage, uchar* outputBuffer);

#endif // !__COMMON_H_
