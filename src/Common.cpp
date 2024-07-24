#include "Common.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstring>
#include "GLErrorDef.h"
#include <GL/gl.h>
#include <opencv2/opencv.hpp>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
bool isTexture(GLuint textureID) {
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Check if the texture is valid
    bool isValid = glIsTexture(textureID) == GL_TRUE;

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    return isValid;
}

void _uploadImageToTexture(const cv::Mat& image, GLuint& textureID) {
    // Check if textureID is already a valid texture
    if (isTexture(textureID)) {
        // Bind the existing texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Update the existing texture with new image data
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.cols, image.rows, GL_RGBA, GL_UNSIGNED_BYTE, image.data);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        return;
    }

    // Generate texture ID
    glGenTextures(1, &textureID);

    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload image data to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_BGRA, GL_UNSIGNED_BYTE, image.data);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void copyImage(const cv::Mat& inputImage, uchar* outputBuffer)
{
    // Check if the input image data is continuous
    if (inputImage.isContinuous())
    {
        // Calculate the total number of elements
        size_t totalSize = inputImage.total() * inputImage.elemSize();

        // Copy the data from the input image to the output buffer
        std::memcpy(outputBuffer, inputImage.data, totalSize);
    }
    else
    {
        // Calculate the total number of rows and columns
        int rows = inputImage.rows;
        int cols = inputImage.cols;

        // Calculate the total number of elements per row
        size_t rowSize = cols * inputImage.elemSize();

        // Copy the data row by row
        for (int i = 0; i < rows; i++)
        {
            std::memcpy(outputBuffer + i * rowSize, inputImage.ptr(i), rowSize);
        }
    }
}




int SetupTexture(GLuint& texture, int width, int height) {
    if(glIsTexture(texture)) {
        glBindTexture(GL_TEXTURE_2D, texture); CHECK_GL;
    } else {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE); CHECK_GL;
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); CHECK_GL;
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr); CHECK_GL;
    }
    
    glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL;
    return 0;
}



int SetupTextureData(GLuint& texture, int width, int height, const void * data) {
    if (glIsTexture(texture)) {
        glBindTexture(GL_TEXTURE_2D, texture); CHECK_GL;
    }
    else {
        glGenTextures(1, &texture); CHECK_GL;
        glBindTexture(GL_TEXTURE_2D, texture); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); CHECK_GL;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); CHECK_GL;
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height); CHECK_GL;
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data); CHECK_GL;
    glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL;
    return 0;
}

int SetupBufferData(GLuint& buffer, int size, const void * data)
{
    if (!glIsBuffer(buffer)) {
        glGenBuffers(1, &buffer); CHECK_GL;
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer); CHECK_GL;
        glBufferData(GL_PIXEL_UNPACK_BUFFER, size, data, GL_STATIC_COPY); CHECK_GL;
    }
    else {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer); CHECK_GL;
        glBufferSubData(GL_PIXEL_UNPACK_BUFFER, 0, size, data); CHECK_GL;
    }
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); CHECK_GL;
    return 0;
}

int SetupSSBO(GLuint& buffer, const void* data, int elemSize, int bufSize){
    if (!glIsBuffer(buffer)) {
        glGenBuffers(1, &buffer); CHECK_GL;
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize * elemSize, data, GL_STATIC_COPY);
    return 0;
}
using std::ifstream;

bool is_file_exist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

