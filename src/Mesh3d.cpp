#include "Mesh3d.h"
#include "Shader.h"
#include "ModelObj.h"
#include "ModelGL.h"
#include "FileConfig.h"
#include <random>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
using MB::math::mix;
using MB::math::floor;
using MB::math::fract;
using MB::math::abs;
using MB::math::sin;
using MB::math::cos;

/*double fract(double num) {
    return num - static_cast<int>(num);
}*/

float hash( vec2f p ) {
	float h = MB::dot(p,vec2f(127.1,311.7));	
    return fract(sin(h)*43758.5453123f);
}

float noise(vec2f p ) {
    vec2f i = floor( p );
    vec2f f = fract( p );	
	vec2f u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2f(0.0,0.0) ), 
                     hash( i + vec2f(1.0,0.0) ), u.x),
                mix( hash( i + vec2f(0.0,1.0) ), 
                     hash( i + vec2f(1.0,1.0) ), u.x), u.y);
}

float sea_octave(vec2f uv, float choppy) {
    uv += noise(uv);  
    vec2f wv = 1.0-abs(sin(uv));
    vec2f swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}




void Mesh3d::loadShader() {
    /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/water.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/water.frag");

    renderShader = std::make_shared<Shader>();
    renderShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str());
    if (!renderShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), renderShader->program);
    }
}

void Mesh3d::loadMesh() {
    string waterModelFile = resourceFolder + "models/plane300x300.obj";
    waterModel.reset((ModelObj*)ModelObj::CreateFromObjFile(waterModelFile.c_str(), 1.0f, false, false, false));
    if (!waterModel) {
        printf("failed to load water model object: %s \n", waterModelFile.c_str());
    }
    else {
        printf("succeeded to load water model object: %s\n", waterModelFile.c_str());
        waterModelGl.reset(ModelGL::CreateFromModel(waterModel.get()));
        if(waterModelGl){
            printf("succeeded to load water model object : %s as GL memory OBject \n", waterModelFile.c_str());
        }
    }
}

void Mesh3d::loadTexture() {
    Sprite2d::loadTexture();
}

void Mesh3d::updateModelViewUI(int w, int h){
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("3D Settings");

    ImGui::Checkbox("UsePoints", &usePoints);
    ImGui::SameLine();
    ImGui::Checkbox("UseLines", &useLines);
    ImGui::SliderFloat("modle scale X", (float *)&model_scale.x, .0f, 20.2f);
    ImGui::SliderFloat("modle scale Z", (float *)&model_scale.z, .0f, 20.2f);
    ImGui::SliderFloat(" Y for wave height", (float *)&model_scale.y, .0f, 0.2f);
    ImGui::SliderFloat3("model rotation", (float *)&model_rot, -PI * 2, PI * 2);
    ImGui::SliderFloat3("modle translation", (float *)&model_translation, -30.f, 30.0f);
    ImGui::SliderFloat3("lookAt vector", (float *)&lookat, -32.0f, 32.0f);
    ImGui::SliderFloat3("eye vector", (float *)&eye, -22.0f, 22.0f);

    ImGui::Text("===============Save|Load Model View settings===================");
    if (ImGui::Button("save ModelView settings"))
    {
        saveModelViewData();
    }
    ImGui::SameLine();
    if (ImGui::Button("Reload ModelView settings"))
    {
        loadModelViewData();
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

    ImGui::Checkbox("show light settings", &show_lighting_settings);
    ImGui::Checkbox("show waves settings", &show_water_settings);
    ImGui::End();
}

void Mesh3d::updateLightsUI(int w, int h)
{
#define LIGHT_SLIDER_POS_ARRAY(i, name) ImGui::SliderFloat3((string("light") + std::to_string(i) + " Position").c_str(), (float*)&name[i], -200, 200);
#define LIGHT_SLIDER_COLOR_ARRAY(i, name) ImGui::SliderFloat3((string("light") + std::to_string(i) + " Color").c_str(), (float*)&name[i], 0, 1);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Begin("Lighting, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("3D light Settings");

    ImGui::Checkbox("useTexture", &useTexture);
    ImGui::Text("===============Save | Load light settings===================");
    if (ImGui::Button("save light settings"))
    {
        saveLightingArrayData();
    }
    ImGui::SameLine();
    if (ImGui::Button("load light settings"))
    {
        loadLightingArrayData();
    }
    ImGui::SameLine();
    if (ImGui::Button("load random light settings"))
    {
        loadRandLightingArrayData();
    }
    for (size_t i = 0; i < lightNum; i++)
    {
        LIGHT_SLIDER_POS_ARRAY(i, lightPositions);
    }
    for (size_t i = 0; i < lightNum; i++)
    {
        LIGHT_SLIDER_COLOR_ARRAY(i, lightColors);
    }

    ImGui::SliderFloat("alphaColor", (float *)&alphaColor, .0f, 1.0f);
    ImGui::SliderFloat("diffusePower", (float *)&model_diffusePower, .0f, 12.0f);
    ImGui::SliderFloat("specularPower", (float *)&model_specularPower, .0f, 1300.0f);

    ImGui::SliderInt("lights", (int *)&lightNum, 0, MAX_LIGHTS);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void Mesh3d::updateWavesUI(int w, int h)
{
#define SLIDER_FLOAT2_ARRAY(i, name) ImGui::SliderFloat2((string("waves") + std::to_string(i) + " Direction").c_str(), (float *)&name[i], -1, 1);
#define SLIDER_FLOAT3_ARRAY(i, name) ImGui::SliderFloat3((string("waves") + std::to_string(i) + " AWP").c_str(), (float *)&name[i], -1, 1);
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::Begin("water world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("water Settings");
    ImGui::SliderFloat("time step", (float *)&timestep, 0.0001, 0.1);
    ImGui::SliderInt("waves", (int *)&waveCount, 0, MAX_WAVES);
    ImGui::SliderFloat("waves power", (float *)&wavePower, 1.0, 10.0);
    ImGui::Text("===============Save | Load settings===================");
    if (ImGui::Button("save waves settings"))
    {
        saveWavesData();
    }
    ImGui::SameLine();
    if (ImGui::Button("load waves settings"))
    {
        loadWavesData();
    }

    if (ImGui::Button("generate random waves settings"))
    {
        loadRandWavesData();
    }
    ImGui::Checkbox("use Transition waves", &transitionWave);
    ImGui::Checkbox("perturbation waves", &wavePerturbation);
    ImGui::Text("===============Direction settings===================");

    for (size_t i = 0; i < waveCount; i++)
    {
        SLIDER_FLOAT2_ARRAY(i, waves_D);
    }

    ImGui::Text("===============AWP settings===================");

    for (size_t i = 0; i < waveCount; i++)
    {
        SLIDER_FLOAT3_ARRAY(i, waves_AWP);
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void Mesh3d::updateUI(int w, int h) {
// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    updateModelViewUI(w, h);
   

    if (show_lighting_settings) {
        updateLightsUI(w, h);
    }
    if (show_water_settings) {
        updateWavesUI(w, h);
    }
}
#define CONFIG_ADD(var) fileConfig.Add((#var), var)
#define CONFIG_READ_VEC3F(var) var = fileConfig.Readvec3f(#var)
#define CONFIG_READ(var) var = fileConfig.Read(#var, var)
void Mesh3d::saveLightingArrayData() {
    string path = resourceFolder + "arraydata.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    for (size_t i = 0; i < MAX_LIGHTS; i++)
    {
        std::string key = std::string("lightColors") + std::to_string(i);
        fileConfig.Add(key, lightColors[i]);
    }

    for (size_t i = 0; i < MAX_LIGHTS; i++)
    {
        std::string key = std::string("lightPositions") + std::to_string(i);
        fileConfig.Add(key, lightPositions[i]);
    }

    CONFIG_ADD(model_diffusePower);
    CONFIG_ADD(model_specularPower);
    fileConfig.Save();

}

#define COLOR_KEY(i)  lightColors[i]
void Mesh3d::loadLightingArrayData() {
    string path = resourceFolder + "arraydata.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    for (size_t i = 0; i < MAX_LIGHTS; i++)
    {
        std::string key = std::string("lightColors") + std::to_string(i);
        lightColors[i] = fileConfig.Readvec3f(key);
    }

    for (size_t i = 0; i < MAX_LIGHTS; i++)
    {
        std::string key = std::string("lightPositions") + std::to_string(i);
        lightPositions[i] = fileConfig.Readvec3f(key);
    }
    CONFIG_READ(model_specularPower);
    CONFIG_READ(model_diffusePower);
    
}

void Mesh3d::loadRandLightingArrayData(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distributionColor(0.1f, 0.3);

    //position
    std::normal_distribution<float> distributionX(0.0f, 100.f);
    std::normal_distribution<float> distributionZ(0.0f, 100.f);
    std::normal_distribution<float> distributionY(80.0f, 10.f);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        lightColors[i].x = abs(distributionColor(gen));
        lightColors[i].y = abs(distributionColor(gen));
        lightColors[i].z = abs(distributionColor(gen));

        lightPositions[i].x = distributionX(gen);
        lightPositions[i].y = distributionY(gen);
        lightPositions[i].z = distributionZ(gen);
    }
    //lightNum = MAX_LIGHTS;
}

void Mesh3d::loadWavesData() {
    string path = resourceFolder + "waves_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    for (size_t i = 0; i < MAX_WAVES; i++)
    {
        std::string key = std::string("waves_D") + std::to_string(i);
        waves_D[i] = fileConfig.Readvec2f(key);
    }

    for (size_t i = 0; i < MAX_WAVES; i++)
    {
        std::string key = std::string("waves_AWP") + std::to_string(i);
        waves_AWP[i] = fileConfig.Readvec3f(key);
    }
    CONFIG_READ(waveCount);
}

void Mesh3d::loadRandWavesData(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution1(0.2f, .5f);
    std::normal_distribution<float> distributionA(0.4f, 1);
    std::normal_distribution<float> distributionW(0.01f, 0.2);
    std::normal_distribution<float> distributionP(0.0f, .8f);


    for (int i = 0; i < MAX_WAVES; ++i) {
        waves_D[i].x = distribution1(gen);
        waves_D[i].y = distribution1(gen);

        waves_AWP[i].x = (distributionA(gen));
        waves_AWP[i].y = abs(distributionW(gen));
        waves_AWP[i].z = abs(distributionP(gen));
    }
    //waveCount = MAX_WAVES;
}

void Mesh3d::updateRandWavesData(int num)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> distribution1(0.2f, .5f);
    std::normal_distribution<float> distributionA(0.4f, 1);
    std::normal_distribution<float> distributionW(0.01f, 0.05);
    std::normal_distribution<float> distributionP(0.0f, .8f);
    if (curPivotWave < 0)
        curPivotWave = 0;
    int i = curPivotWave;
    int count = num;
    while(count > 0) {
        new_waves_D[i].x = distribution1(gen);
        new_waves_D[i].y = distribution1(gen);

        new_waves_AWP[i].x = (distributionA(gen));
        new_waves_AWP[i].y = abs(distributionW(gen));
        new_waves_AWP[i].z = abs(distributionP(gen));
        old_waves_D[i] = waves_D[i];
        old_waves_AWP[i] = waves_AWP[i];
        i = (i + 1) % waveCount;
        --count;
    }
    curPivotWave = (curPivotWave + num) % waveCount;
}

void Mesh3d::transitionWavesValue(int start, int num, float t)
{
    int i = start;
    int count = num;
    while (count > 0) {
        waves_D[i] = new_waves_D[i]*t + old_waves_D[i] * (1.0 - t);
        waves_AWP[i] = new_waves_AWP[i] * t + old_waves_AWP[i] * (1.0 - t);
        waves_AWP[i].z = new_waves_AWP[i].z;
        i = (i + 1) % waveCount;
        --count;
    }
}

void Mesh3d::saveWavesData() {
    string path = resourceFolder + "waves_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    for (size_t i = 0; i < MAX_WAVES; i++)
    {
        std::string key = std::string("waves_D") + std::to_string(i);
        fileConfig.Add(key, waves_D[i]);
    }

    for (size_t i = 0; i < MAX_WAVES; i++)
    {
        std::string key = std::string("waves_AWP") + std::to_string(i);
        fileConfig.Add(key, waves_AWP[i]);
    }
    CONFIG_ADD(waveCount);
    fileConfig.Save();
}


void Mesh3d::saveModelViewData(){
    string path = resourceFolder + "model_view_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    CONFIG_ADD(model_scale);
    CONFIG_ADD(model_translation);
    CONFIG_ADD(model_rot);
    CONFIG_ADD(lookat);
    CONFIG_ADD(eye);

    fileConfig.Save();
}

void Mesh3d::loadModelViewData(){
    string path = resourceFolder + "model_view_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);
    CONFIG_READ_VEC3F(model_scale);
    CONFIG_READ_VEC3F(model_translation);
    CONFIG_READ_VEC3F(model_rot);
    CONFIG_READ_VEC3F(lookat);
    CONFIG_READ_VEC3F(eye);
}

void Mesh3d::drawSprite(int w, int h, vec2f offset) {
    vec3f eye = vec3f(this->eye.x, this->eye.y, this->eye.z);
    vec3f at(lookat.x, lookat.y, lookat.z);
    vec3f up(0, 1, 0);

    float w_view = 1.0f, h_view = 1.0f;
    float near = 1;      
    float far = 10000.0f;
    float aspect = w_view / h_view;
    float fovy = PI / 2.0;

    matrix4f model, view, projection;
    matrix4f r, rx, ry, rz, t, s;
    r = MB::rotationY(r, 0.0f);
    t.set_translate(model_translation);
    s.set_scale(model_scale);
    rx = MB::rotationX(rx, model_rot.x);
    ry = MB::rotationY(ry, model_rot.y);
    rz = MB::rotationZ(rz, model_rot.z);
    r = rx * ry * rz;
    model = t * r * s;

    MB::lookAt(view, eye, at, up);
    MB::perspective(projection, fovy,
        aspect,
        near, far);
    float specularPower = model_specularPower;
    float diffusePower = model_diffusePower;
    
    renderShader->Use(12);
    renderShader->setUniformMatrix4fv("model", model._array, 1, GL_FALSE);
    renderShader->setUniformMatrix4fv("view", view._array, 1, GL_FALSE);
    renderShader->setUniformMatrix4fv("projection", projection._array, 1, GL_FALSE);
    renderShader->setUniform1f("specularPower", specularPower);
    renderShader->setUniform1f("diffusePower", diffusePower);
    renderShader->setUniform1i("useTexture", useTexture);
    renderShader->setUniform1i("lightNum", lightNum);
    renderShader->setUniform3fv("lights", (const float*)lightPositions, lightNum);
    renderShader->setUniform3fv("lights_Color", (const float*)lightColors, lightNum);
    renderShader->setUniform1f("alphaColor", alphaColor);

    glBindTexture(GL_TEXTURE_2D, ourTexture);
    waterModelGl->drawElements(0, 2, 1, usePoints, useLines);
}

void Mesh3d::stepSimulation(float w, float h, float dt) {
    static float time = 0;
    static int cycle = 0;
    time += dt;
    ++cycle;
    const int num = 1;
    const int cycleCount = 300;
    if (transitionWave) {
        if (cycle % cycleCount == 0) {
            updateRandWavesData(num);
        }
        else if (curPivotWave >= 0) {
            int start = (curPivotWave - num + waveCount) % waveCount;
            float t = 1.0 * (cycle % cycleCount) / cycleCount;
            transitionWavesValue(start, num, t);
        }
    }


    renderShader->Use(12);
    renderShader->setUniform1f("time", time);
    renderShader->setUniform1f("waves_Power", wavePower);
    renderShader->setUniform1i("waveCount", waveCount);
    renderShader->setUniform1i("wavePerturbation", wavePerturbation);
    renderShader->setUniform2fv("waves_D", (const float*)waves_D, waveCount);
    renderShader->setUniform3fv("waves_AWP", (const float*)waves_AWP, waveCount);
}

void Mesh3d::initSimulation(int w, int h) {
    for (size_t i = 0; i < MAX_LIGHTS; i++)
    {
        lightColors[i] = vec3f(0.f);
        lightPositions[i]  = vec3f(0.f, 200.f, 0.f);
    }
    
    lightColors[0] = vec3f(0.5, 0.5, 0.5);
    lightColors[1] = vec3f(0.6, 0.6, 0.6);
    lightColors[2] = vec3f(0.5, 0.5, 0.5);
    lightColors[3] = vec3f(0.5, 0.5, 0.5);
    lightColors[4] = vec3f(0.5, 0.5, 0.5);

    lightPositions[0] = vec3f(200, 200, 200);
    lightPositions[1] = vec3f(-200, 200, 100);
    lightPositions[2] = vec3f(0, 100, -100);
    lightPositions[3] = vec3f(200, 200, 200);
    lightPositions[4] = vec3f(200, 200, 200);

    loadLightingArrayData();
    loadWavesData();
    loadModelViewData();

}

void Mesh3d::run(float w, float h)
{
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(false);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glColorMask(true, true, true, false);
    
	stepSimulation(w, h, timestep);
    glViewport(0, 0, w, h);
	drawSprite(w, h, vec2f(0,0));
	
    updateUI(w, h);

    glDisable(GL_BLEND);
    glColorMask(true, true, true, true);
}
