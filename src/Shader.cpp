#include "Shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
/*#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)*/
#define VLOGI   LOGI
#define VLOGD   LOGD
#define VLOGE   printf
GLuint Shader::LoadShader(GLenum shader_type, const char *source)
{
    GLuint shader = glCreateShader(shader_type);
    CHECK_GL;
    if (shader)
    {
        GLint compiled = 0;
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        CHECK_GL;
        if (!compiled)
        {
            GLint len = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

            if (len)
            {
                char *buf = (char *)malloc(len);
                if (buf)
                {
                    glGetShaderInfoLog(shader, 512, NULL, buf);
                    VLOGE("%s: Could not compile shader %d: %s", __func__, shader_type, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                CHECK_GL;
                shader = 0;
            }
        }
    }
    else
    {
    }
    return shader;
}
GLuint Shader::CreateProgram(const char *vertex_source, const char *fragment_source, const char *compute_source)
{
    GLuint computeShader = 0;
    GLuint vertexShader = 0;
    GLuint pixelShader = 0;

    if (compute_source)
    {
        computeShader = LoadShader(GL_COMPUTE_SHADER, compute_source);
        if (!computeShader)
        {
            VLOGE("Load compute shader failed: content:\n %s\n", compute_source);
            VLOGE("end=============== \n");
            return 0;
        }
    }
    else
    {
        vertexShader = LoadShader(GL_VERTEX_SHADER, vertex_source);
        if (!vertexShader)
        {
            VLOGE("Load vertex shader failed===== %s\n", vertex_source);
            VLOGE("end=============== \n");
            return 0;
        }

        pixelShader = LoadShader(GL_FRAGMENT_SHADER, fragment_source);
        if (!pixelShader)
        {
            VLOGE("Load pixel shader failed====== %s\n", fragment_source);
            VLOGE("end=============== \n");
            return 0;
        }
    }

    program = glCreateProgram();
    CHECK_GL;
    if (!program)
    {
        VLOGE("%s: Create program failed", __func__);
        return 0;
    }

    if (computeShader)
    {
        glAttachShader(program, computeShader);
        CHECK_GL;
    }
    else
    {
        glAttachShader(program, vertexShader);
        CHECK_GL;
        glAttachShader(program, pixelShader);
        CHECK_GL;
    }

    GLint status = GL_FALSE;
    glLinkProgram(program);
    CHECK_GL;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    CHECK_GL;
    if (status != GL_TRUE)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        CHECK_GL;
        if (len)
        {
            char *buf = (char *)malloc(len);
            if (buf)
            {
                glGetProgramInfoLog(program, len, &len, buf);
                CHECK_GL;
                VLOGE("%s: Could not link program: %s", "__func__", buf);
                free(buf);
            }
        }
        glDeleteProgram(program);
        CHECK_GL;
        program = 0;
    }

    return program;
}
// Constructor generates the shader on the fly
bool Shader::setShader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar* geometryPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream geometryFile;
    // ensures ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    geometryFile.exceptions(std::ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        if(geometryPath != NULL){
           geometryFile.open(geometryPath); 
        }
        
        std::stringstream vShaderStream, fShaderStream, geometryStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        if(geometryPath != NULL){
            geometryStream << geometryFile.rdbuf();
        }
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        if(geometryPath != NULL){
           geometryFile.close();
        }
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        geometryCode = geometryStream.str();
        /*cout << "vertexCode:  " << endl
             << vertexCode << endl;
        cout << "fragmentCode:  " << endl
             << fragmentCode << endl;*/
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return false;
    }
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar* gShaderCode = geometryCode.c_str();
    // 2. Compile shaders
    GLuint vertex, fragment, geometry = 0;
    GLint success;
    GLchar infoLog[512];
    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        printf("vertexCode    %s\n", vertexCode.c_str());
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        glDeleteShader(vertex);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        printf("%s\n", fragmentCode.c_str());
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }
    if (!geometryCode.empty()) {
        // Fragment Shader
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        // Print compile errors if any
        glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            printf("%s\n", geometryCode.c_str());
            glGetShaderInfoLog(geometry, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n"
                << infoLog << std::endl;
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            glDeleteShader(geometry);
            return false;
        }
    }
    // Shader Program
    this->program = glCreateProgram();
    glAttachShader(this->program, vertex);
    glAttachShader(this->program, fragment);
    if (geometry > 0) {
        glAttachShader(this->program, geometry);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry > 0) {
        glDeleteShader(geometry);
    }
    glLinkProgram(this->program);
    // Print linking errors if any
    glGetProgramiv(this->program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->program, 512, NULL, infoLog);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometry > 0) {
            glDeleteShader(geometry);
        }
        glDeleteProgram(program);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
        return false;
    }
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry > 0) {
        glDeleteShader(geometry);
    }

    return true;
}

bool Shader::setComputeShader(const GLchar* shaderPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string shaderCode;
    std::ifstream ShaderFile;

    // ensures ifstream objects can throw exceptions:
    ShaderFile.exceptions(std::ifstream::badbit);
    try
    {
        // Open files
        ShaderFile.open(shaderPath);
        std::stringstream ShaderStream;
        // Read file's buffer contents into streams
        ShaderStream << ShaderFile.rdbuf();
        // close file handlers
        ShaderFile.close();
        // Convert stream into string
        shaderCode = ShaderStream.str();
        /*cout << "shaderCode:  " << endl
             << shaderCode << endl;*/
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return false;
    }
    const GLchar *computeShaderStr = shaderCode.c_str();

    GLint computeShader = LoadShader(GL_COMPUTE_SHADER, computeShaderStr);
    if (!computeShader)
    {
        VLOGE("Load compute shader failed %s\n", computeShaderStr);
        return 0;
    }

    program = glCreateProgram();
    CHECK_GL;
    if (!program)
    {
        VLOGE("%s: Create program failed", __func__);
        return 0;
    }

    if (computeShader)
    {
        glAttachShader(program, computeShader);
        CHECK_GL;
    }

    GLint status = GL_FALSE;
    glLinkProgram(program);
    CHECK_GL;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    CHECK_GL;
    if (status != GL_TRUE)
    {
        GLint len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        CHECK_GL;
        if (len)
        {
            char *buf = (char *)malloc(len);
            if (buf)
            {
                glGetProgramInfoLog(program, len, &len, buf);
                CHECK_GL;
                VLOGE("%s: Could not link program: %s", __func__, buf);
                free(buf);
            }
        }
        glDeleteProgram(program);
        CHECK_GL;
        program = 0;
    }

    return program;
}