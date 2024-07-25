#pragma once
#include "Common.h"
#include "Sprite2d.h"
#include <string>
#include "NvMath.h"

using MB::vec2i;
using MB::vec2f;
using MB::vec4f;

class SKShader;
class SKModel;
class Animation;
class Animator;
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

    SKModel* ourModel;
    Animation* danceAnimation;
    Animator* animator;

    // timing
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
protected:
    void loadShader();
    void loadMesh();
    void loadTexture();

    void updateUI(int w, int h);

public:
    void init(int w, int h);
    void run(float w, float h);


};