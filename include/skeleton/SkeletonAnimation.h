#pragma once
#include "Common.h"
#include "Sprite2d.h"
#include <string>
#include "NvMath.h"
#include "camera.h"

using MB::vec2i;
using MB::vec2f;
using MB::vec4f;
using MB::vec3f;

class SKShader;
class SKModel;
class Animation;
class Animator;
#define MAX_LIGHTS 15
class SkeletonAnimation: public Sprite2d
{
public:
//locate the sprite2d in screen view coordinate
    int pixelWidth, pixHeight;
    vec2i pixelPosition;

protected:
    std::shared_ptr<SKShader> renderShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    GLuint writeTexture = 0;
    GLuint ourTexture = 0;
    bool useTexture = 0;

    SKModel* ourModel;
    Animation* danceAnimation;
    Animator* animator;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;


    //model loc, ori
    // 
    glm::vec3 modelTranslation;
        //light
    vec3f lightColors[MAX_LIGHTS];
    vec3f lightPositions[MAX_LIGHTS];
    float model_diffusePower = 10;
    float model_specularPower = 10;
    int lightNum = 3;
    float alphaColor = 1.0;
    float modelScale = 1.0;
    Camera camera;
protected:
    void loadShader();
    void loadMesh();
    void loadTexture();

    void updateUI(int w, int h);
    virtual void updateLightsUI(int w, int h);
    virtual void onScroll(float dxScreen, float dyScreen);
	virtual void onFling(float vx, float vy);

public:
    void init(int w, int h);
    void run(float w, float h);


};