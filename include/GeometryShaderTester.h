#pragma once
#include "Common.h"
#include "ShaderTester.h"
#include "RenderState.h"
#include "CubicSpline.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>
using std::string;
class Shader;
#define MAX_RESULT_LEN 10



class GeometryShaderTester : public ShaderTester

{
protected:
    int shaderType = 1;
    std::shared_ptr<Shader> geometryShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    //render states
    float zRotation = 0;
    int usingGeoShader = 0;
    float lineWidth = 0.1;
    int enableGLMultisample = 0;
    //0: GL_TRIANGLES   1:  GL_LINE_STRIP_ADJACENCY  2:GL_POINTS  
    int drawMode = 0;

    std::vector<vec3f> pointsOfCurve;

    //GUI
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 clear_pos = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    void loadAssets0();
    void loadAssets1();
    void loadRenderState();
    void generateTrackPoints();
    void generateTrackPointsByCubicSpline();
    void generateTrackPointsByBeZierCurve();
    void generateTrackPointsByBSpline();
    void renderFrame0(int w, int h);
    void renderFrame1(int w, int h);
    void updateUI(int w, int h);
public:
    void init();
    void run(float w, float h);
};

