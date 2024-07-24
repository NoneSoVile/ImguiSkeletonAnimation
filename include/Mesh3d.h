#pragma once
#include "Sprite2d.h"
class ModelObj;
class ModelGL;
#define MAX_LIGHTS 15
class Mesh3d : public Sprite2d {
protected:
    std::shared_ptr<ModelObj> waterModel;
	std::shared_ptr<ModelGL> waterModelGl;

    //gui data
    bool usePoints = false;
    bool useLines = false;
    bool show_lighting_settings = 0;
    bool show_water_settings = 0;
    bool useTexture = 0;
    vec3f eye = vec3f(0.f, 1.f, 3.f);
    vec3f lookat;
    vec3f model_rot;
    vec3f model_scale= vec3f(3, 1.f, 3.f);
    vec3f model_translation;

    //light
    vec3f lightColors[MAX_LIGHTS];
    vec3f lightPositions[MAX_LIGHTS];
    float model_diffusePower = 10;
    float model_specularPower = 10;
    int lightNum = 3;
    float alphaColor = 1.0;

    //simualation
#define MAX_WAVES 150
    vec2f waves_D[MAX_WAVES];
    vec3f waves_AWP[MAX_WAVES];

    vec2f new_waves_D[MAX_WAVES];
    vec3f new_waves_AWP[MAX_WAVES];
    vec2f old_waves_D[MAX_WAVES];
    vec3f old_waves_AWP[MAX_WAVES];
    int waveCount = 3;
    float timestep = 0.01;
    float wavePower = 1.0;
    int curPivotWave = -1;
    int transitionCount = 0;
    bool transitionWave = 0;
    bool wavePerturbation = 0;
protected:
    virtual void loadShader();
    virtual void loadMesh();
    virtual void loadTexture();

    virtual void updateUI(int w, int h);
    virtual void updateModelViewUI(int w, int h);
    virtual void updateLightsUI(int w, int h);
    virtual void updateWavesUI(int w, int h);

    virtual void drawSprite(int w, int h, vec2f offset);
    virtual void stepSimulation(float w, float h,float dt);
    virtual void initSimulation(int w, int h);

    void saveLightingArrayData();
    void loadLightingArrayData();
    void loadRandLightingArrayData();
    virtual void saveWavesData();
    virtual void loadWavesData();
    void loadRandWavesData();
    void updateRandWavesData(int num);
    void transitionWavesValue(int start, int num, float t);
    virtual void saveModelViewData();
    virtual void loadModelViewData();

public:
    virtual void run(float w, float h);

};