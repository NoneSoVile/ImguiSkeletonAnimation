#include "WaterShader.h"
#include "Shader.h"
#include "ModelObj.h"
#include "ModelGL.h"
#include "FileConfig.h"
#include <random>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"

void WaterShader::loadShader() {
    /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/water2.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/water2.frag");

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

void WaterShader::loadMesh() {
    string waterModelFile = resourceFolder + "models/plane100x100.obj";
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

void WaterShader::loadTexture() {
    Sprite2d::loadTexture();
}


#define CONFIG_ADD(var) fileConfig.Add((#var), var)



//#define CONFIG_ADD_LIST(var1, ...) fileConfig.Add((#var1), var1);CONFIG_ADD1_IN_LIST(__VA_ARGS__);
#define CONFIG_READ_VEC3F(var) var = fileConfig.Readvec3f(#var)
#define CONFIG_READ(var) var = fileConfig.Read(#var, var)

#define SET_UNIFORM_1F(name, value) renderShader->setUniform1f(#name, value)
#define SET_UNIFORM_1I(name, value)  renderShader->setUniform1i(#name, value)
#define SET_UNIFORM_3FV(name, value, number)    renderShader->setUniform3fv(#name, (const float*)value, number);       
#define SET_UNIFORM_MAT_4FV(name, value, number) renderShader->setUniformMatrix4fv(#name, value._array, number, GL_FALSE);    
void WaterShader::drawSprite(int w, int h, vec2f offset) {
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
    /*renderShader->setUniform1f("specularPower", specularPower);
    renderShader->setUniform1f("diffusePower", diffusePower);
    renderShader->setUniform1i("useTexture", useTexture);
    renderShader->setUniform1i("lightNum", lightNum);
    renderShader->setUniform3fv("lights", (const float*)lightPositions, lightNum);
    renderShader->setUniform3fv("lights_Color", (const float*)lightColors, lightNum);
    renderShader->setUniform1f("alphaColor", alphaColor);*/

    glBindTexture(GL_TEXTURE_2D, ourTexture);
    waterModelGl->drawElements(0, 2, 1, usePoints, useLines);
}

void WaterShader::stepSimulation(float w, float h, float dt) {
    static float time = 0;
    static int cycle = 0;
    time += dt;

    renderShader->Use(12);
    SET_UNIFORM_1F(time, time);
    SET_UNIFORM_1I(NUM_STEPS, NUM_STEPS);
    SET_UNIFORM_1I(ITER_GEOMETRY, ITER_GEOMETRY);
    SET_UNIFORM_1I(ITER_FRAGMENT, ITER_FRAGMENT);
    SET_UNIFORM_1F(SEA_HEIGHT, SEA_HEIGHT);
    SET_UNIFORM_1F(SEA_CHOPPY, SEA_CHOPPY);
    SET_UNIFORM_1F(SEA_SPEED, SEA_SPEED);
    SET_UNIFORM_1F(SEA_FREQ, SEA_FREQ);
    SET_UNIFORM_3FV(SEA_BASE, SEA_BASE, 1);
    SET_UNIFORM_3FV(SEA_WATER_COLOR, &SEA_WATER_COLOR, 1);
    SET_UNIFORM_1I(noiseFunc, noiseFunc);

}

void WaterShader::saveWavesData() {
    string path = resourceFolder + "watershader/waves_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);


    CONFIG_ADD(waveCount);
    CONFIG_ADD(NUM_STEPS);
    CONFIG_ADD(ITER_GEOMETRY);
    CONFIG_ADD(ITER_FRAGMENT);
    CONFIG_ADD(SEA_HEIGHT);
    CONFIG_ADD(SEA_CHOPPY);
    CONFIG_ADD(SEA_SPEED);
    CONFIG_ADD(SEA_FREQ);
    CONFIG_ADD(SEA_BASE);
    CONFIG_ADD(SEA_WATER_COLOR);
    
    fileConfig.Save();
}



void WaterShader::loadWavesData() {
    string path = resourceFolder + "watershader/waves_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    CONFIG_READ(waveCount);
    CONFIG_READ(NUM_STEPS);
    CONFIG_READ(ITER_GEOMETRY);
    CONFIG_READ(ITER_FRAGMENT);
    CONFIG_READ(SEA_HEIGHT);
    CONFIG_READ(SEA_CHOPPY);
    CONFIG_READ(SEA_SPEED);
    CONFIG_READ(SEA_FREQ);
    CONFIG_READ_VEC3F(SEA_BASE);
    CONFIG_READ_VEC3F(SEA_WATER_COLOR);
    CONFIG_READ(waveCount);
}

void WaterShader::saveModelViewData(){
    string path = resourceFolder + "watershader/model_view_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);

    CONFIG_ADD(model_scale);
    CONFIG_ADD(model_translation);
    CONFIG_ADD(model_rot);
    CONFIG_ADD(lookat);
    CONFIG_ADD(eye);

    fileConfig.Save();
}

void WaterShader::loadModelViewData(){
    string path = resourceFolder + "watershader/model_view_data.txt";
    Config fileConfig;
    fileConfig.LoadConfig(path);
    CONFIG_READ_VEC3F(model_scale);
    CONFIG_READ_VEC3F(model_translation);
    CONFIG_READ_VEC3F(model_rot);
    CONFIG_READ_VEC3F(lookat);
    CONFIG_READ_VEC3F(eye);
}

void WaterShader::updateUI(int w, int h){
    Mesh3d::updateUI(w, h);
}

void WaterShader::updateWavesUI(int w, int h){
#define SLIDER_FLOAT2_ARRAY(i, name) ImGui::SliderFloat2((string("waves") + std::to_string(i) + " Direction").c_str(), (float *)&name[i], -1, 1);
#define SLIDER_FLOAT3_ARRAY(i, name) ImGui::SliderFloat3((string("waves") + std::to_string(i) + " AWP").c_str(), (float *)&name[i], -1, 1);
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::Begin("water world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("water Settings");
    ImGui::SliderFloat("time step", (float *)&timestep, 0.0001, 0.1);
    ImGui::SliderInt("ITER_GEOMETRY", (int *)&ITER_GEOMETRY, 1, 20);
    ImGui::SliderInt("ITER_FRAGMENT", (int *)&ITER_FRAGMENT, 1, 20);


    ImGui::SliderFloat("SEA_HEIGHT", (float *)&SEA_HEIGHT, 0.1, 10.0);
    ImGui::SliderFloat("SEA_CHOPPY", (float *)&SEA_CHOPPY, 1.0, 30.0);
    ImGui::SliderFloat("SEA_SPEED", (float *)&SEA_SPEED, 0.1, 10.0);
    ImGui::SliderFloat("SEA_FREQ", (float *)&SEA_FREQ, 0.01, 3.0);

    ImGui::SliderFloat3("SEA_BASE", (float *)&SEA_BASE, 0.0, 1.0);
    ImGui::SliderFloat3("SEA_WATER_COLOR", (float *)&SEA_WATER_COLOR, 0.0, 19.0);
    ImGui::SliderInt("noise Func", (int *)&noiseFunc, 0, 10);
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



    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}


