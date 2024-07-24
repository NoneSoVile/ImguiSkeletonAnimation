#pragma once
#include "Mesh3d.h"
class ModelObj;
class ModelGL;
#define MAX_LIGHTS 15
class WaterShader : public Mesh3d {
protected:
    virtual void loadShader();
    virtual void loadMesh();
    virtual void loadTexture();

    virtual void drawSprite(int w, int h, vec2f offset);
    virtual void stepSimulation(float w, float h,float dt);

    virtual void saveWavesData();
    virtual void loadWavesData();

    virtual void saveModelViewData();
    virtual void loadModelViewData();

    virtual void updateUI(int w, int h);
    virtual void updateWavesUI(int w, int h);

protected:
    //data of waves
     int NUM_STEPS = 8;
     int ITER_GEOMETRY = 3;
     int ITER_FRAGMENT = 5;
     float SEA_HEIGHT = 1.5;
     float SEA_CHOPPY = 14.0;
     float SEA_SPEED = 1.8;
     float SEA_FREQ = 0.056;
     vec3f SEA_BASE = vec3f(0.0, 0.09, 0.18);
     vec3f SEA_WATER_COLOR = vec3f(0.8, 0.9, 0.6) * 0.6;
     int noiseFunc = 0;
};