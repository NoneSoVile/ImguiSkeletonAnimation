#include "SkeletonAnimation.h"
#include "sk_Shader.h"

#include <animator.h>
#include <model_animation.h>
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>
#include "camera.h"

void SkeletonAnimation::loadShader(){
     /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/anim_model.vs");
    string fragShaderFile = resourceFolder + std::string("shaders/basic3d.frag");

    renderShader = std::make_shared<SKShader>(vertexShaderFile.c_str(), fragShaderFile.c_str());
    //renderShader->(vertexShaderFile.c_str(), fragShaderFile.c_str());//geometryShaderFile.c_str()
    if (!renderShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), renderShader->getProgramID());
    }

}

void SkeletonAnimation::init(int w, int h) {
	camera = Camera(glm::vec3(0.0f, 0.0f, 5.0f));
	for (size_t i = 0; i < MAX_LIGHTS; i++)
	{
		lightPositions[i].z = 30;
	}
	
     /*create shaders
    */
    loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();
}

void SkeletonAnimation::loadMesh()
{  
    //vampire/dancing_vampire.dae"
    //minecraft/minecraft.dae
    ourModel = new SKModel(("resource/models/astro_max.dae"));
	danceAnimation = new Animation(("resource/models/astro_max.dae"), ourModel);
	animator = new Animator(danceAnimation);

}

void SkeletonAnimation::loadTexture()
{
    string name = "resource/images/car.jpg";
	cv::Mat img = cv::imread(name, cv::IMREAD_UNCHANGED);
	if (img.empty())
	{
		printf("failed on opening  texture image %s \n", name.c_str());
		return;
	}
	cv::cvtColor(img, img, cv::COLOR_RGB2BGRA);
	cv::Size s = img.size();  
	int h = s.height;
	int w = s.width;

	SetupTextureData(ourTexture, w, h, img.data);
	printf("Created a new texture with id = %d h = %d, w = %d\n", ourTexture, h, w);
}

void SkeletonAnimation::updateUI(int w, int h) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.

        ImGui::SameLine();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

	updateLightsUI(w, h);
}

void SkeletonAnimation::updateLightsUI(int w, int h)
{
#define LIGHT_SLIDER_POS_ARRAY(i, name) ImGui::SliderFloat3((string("light") + std::to_string(i) + " Position").c_str(), (float*)&name[i], -30, 30);
#define LIGHT_SLIDER_COLOR_ARRAY(i, name) ImGui::SliderFloat3((string("light") + std::to_string(i) + " Color").c_str(), (float*)&name[i], 0, 1);
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::Begin("Lighting, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);
    ImGui::Text("3D light Settings");

   	ImGui::Checkbox("useTexture", &useTexture);
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
    ImGui::SliderFloat("modelScale", (float*)&modelScale, .0f, 25.0f);

    ImGui::SliderInt("lights", (int *)&lightNum, 0, MAX_LIGHTS);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
}

void SkeletonAnimation::run(float w, float h) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
	float specularPower = model_specularPower;
    float diffusePower = model_diffusePower;

    glViewport(0, 0, w, h);
    animator->UpdateAnimation(deltaTime);
    renderShader->use();
	renderShader->setFloat("specularPower", specularPower);
    renderShader->setFloat("diffusePower", diffusePower);
    renderShader->setInt("useTexture", useTexture);
    renderShader->setInt("lightNum", lightNum);
    renderShader->setVec3(("lights"), (float*)lightPositions, lightNum);
    renderShader->setVec3("lights_Color", (float*)lightColors, lightNum);
    //renderShader->setFloat("alphaColor", alphaColor);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(1.0f, w / h, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    renderShader->setMat4("projection", projection);
    renderShader->setMat4("view", view);

    auto transforms = animator->GetFinalBoneMatrices();
    for (int i = 0; i < transforms.size(); ++i)
        renderShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);


    // render the loaded model
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(.5f, .5f, .5f)* modelScale);	// it's a bit too big for our scene, so scale it down
    renderShader->setMat4("model", model);
    ourModel->Draw(*renderShader);

    updateUI(w, h);
}


void SkeletonAnimation::onScroll(float dxScreen, float dyScreen){
	camera.RotateCameraByMouseMove(dxScreen, dyScreen);
}

void SkeletonAnimation::onFling(float vx, float vy){

}
