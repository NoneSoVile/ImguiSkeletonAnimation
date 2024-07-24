#include "Sprite2d.h"
#include "Shader.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include "imgui_internal.h"
#include <string>

class Shader;


void Sprite2d::init(int w, int h) {
    /*load shaders*/
	loadShader();

    /*create vertices*/
    loadMesh();


    /*create and load textures*/
    loadTexture();

	/*simulation initialization for sprite*/
	initSimulation(w, h);
}

void Sprite2d::loadShader(){
 	/*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/basic.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/basic.frag");

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

void Sprite2d::loadMesh()
{
    GLfloat vertices[] = {
		1.0f, -1.0f, 0.0f, 1.0f, 1.0f,    //RB       from the view direction of -z
		1.0f, 1.0f, 0.0f, 1.0f, 0.0f,     //RT 
		-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,    //LT
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f     //LB
	};

	GLuint indices[] = {
	0,1,3,            
	1,2,3             
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1 , &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); CHECK_GL;

	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER , EBO );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER , sizeof(indices) , indices , GL_STATIC_DRAW ); CHECK_GL;

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0); CHECK_GL;

	// TexCoord attribute
	glVertexAttribPointer(1 , 2 , GL_FLOAT , GL_FALSE , 5*sizeof(GLfloat) , (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1); CHECK_GL;

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
	CHECK_GL;
}

void Sprite2d::loadTexture()
{
    string name = "resource/images/steelball.png";
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

void Sprite2d::updateUI(int w, int h) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::SetNextWindowBgAlpha(0.5f);
    
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.
        ImGui::Text("Settings");               // Display some text (you can use a format strings too)

		if (ImGui::RadioButton("Euler", solve_type == Euler)) { solve_type = Euler; } ImGui::SameLine();
		if (ImGui::RadioButton("improved Euler", solve_type == IMPROVED_EULER)) { solve_type = IMPROVED_EULER; } ImGui::SameLine();
		if (ImGui::RadioButton("RK2", solve_type == RK2)) { solve_type = RK2; }ImGui::SameLine();
		if (ImGui::RadioButton("RK4", solve_type == RK4)) { solve_type = RK4; }

		ImGui::SliderFloat2("sprite size", pixelSize, 2, 960);
		ImGui::SliderFloat2("sprite position", pixelPosition, 0, screenDim.x);
        ImGui::SliderFloat("Trush x direction", &T[0], -200.0f, 200.0f); 
		ImGui::SliderFloat("Trush y direction", &T[1], -200.0f, 200.0f);
		ImGui::SliderFloat("Velocity x direction", &V.x, -200.0f, 200.0f);
		ImGui::SliderFloat("Velocity y direction", &V.y, -200.0f, 200.0f);
		ImGui::SliderFloat("friction coefficient", &C, 0.01f, 60.0f);
		ImGui::SliderFloat("Step Time", &deltaT, 0.5f, 10.0f);
        
		ImGui::NewLine();ImGui::NewLine();
		if (ImGui::Button("zero X Simulation"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			V.x = 0; T[0] = 0;
		}

		if (ImGui::Button("zero y Simulation"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			V.y = 0; T[1] = 0;
		}
		if (ImGui::Button("Reset Simulation"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		{
			initSimulation(screenDim.x, screenDim.y);
		}

		
       

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }
}

void Sprite2d::drawSprite(int w, int h, vec2f offset){
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    renderShader->Use();
    renderShader->UseAndBindTexture("ourTexture", ourTexture);
	renderShader->setUniform2fv("scale", scale, 1);
	renderShader->setUniform2fv("translation", translation + offset, 1);
	glBindVertexArray(VAO); CHECK_GL;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); CHECK_GL;
	GLuint positionHandle = 0;
	GLuint texcoordHandle = 1;
    glEnableVertexAttribArray(positionHandle);
    glEnableVertexAttribArray(texcoordHandle);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); CHECK_GL;

    glDisableVertexAttribArray(positionHandle);
	glDisableVertexAttribArray(texcoordHandle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
}

vec2f Sprite2d::ndcToScreen(vec2f ndcPosition){
	vec2f outPos;
	ndcPosition.y *= -1;
	outPos = ndcPosition * screenDim / 2.0 + screenDim/2.0;
	return outPos;
}
    
vec2f Sprite2d::screenToNdc(vec2f screenPosition){
	vec2f outPos;
	outPos = (2.0*screenPosition - screenDim) /screenDim;
	outPos.y *= -1;
	return outPos;
}


void Sprite2d::initSimulation(int w, int h){
	screenDim = vec2f(w, h);
	pixelSize = vec2f(40, 40);
	pixelPosition = screenDim / 2;

	T = vec2f(2, 2);  //N
	V = vec2f(1, 1);   //Initial velocity
	C = 5;
	M = 10;  //Mass of sprite
	S = vec2f(0, 0);

	for (size_t i = 0; i < MAX_PARTICLES; i++)
	{
		particles_offset[i] = i*80;
	}
}

 void Sprite2d::step0(float dt){
	vec2f Vnew, Snew;
	// Calculate the total force
	F = (T - (C * V ));//** V* vec2f(ImSign(V.x), ImSign(V.y)))
	// Calculate the acceleration
	A = F / M;
	// Calculate the new velocity at time t + dt
	// where V is the velocity at time t
	Vnew = V + A * dt;
	// Calculate the new displacement at time t + dt
	// where S is the displacement at time t
	Snew = S + Vnew * dt;
	// Update old velocity and displacement with the new ones
	V = Vnew;
	S = Snew;
 }

  void Sprite2d::step1(float dt){
	  float eto = 0.01; // truncation error tolerance
	  vec2f V1, V2;
	  vec2f Vnew, Snew;
	  float et; // truncation error
	  // Take one step of size dt to estimate the new velocity
	  F = (T - (C * V));
	  A = F / M;
	  V1 = V + A * dt;
	  // Take two steps of size dt/2 to estimate the new velocity
	  F = (T - (C * V));
	  A = F / M;
	  V2 = V + A * (dt / 2);
	  F = (T - (C * V2));

	  A = F / M;
	  V2 = V2 + A * (dt / 2);
	  // Estimate the truncation error
	  et = MB::length(V1 - V2);
	  // Estimate a new step size
	  float dtnew = dt * sqrt(eto / et);
	  if (dtnew < dt)
	  { // take at step at the new smaller step size
		  F = (T - (C * V));
		  A = F / M;
		  Vnew = V + A * dtnew;
		  Snew = S + Vnew * dtnew;
	  }
	  else
	  { // original step size is okay
		  Vnew = V1;
		  Snew = S + Vnew * dt;
	  }
	  // Update old velocity and displacement with the new ones
	  V = Vnew;
	  S = Snew;
 }

 void Sprite2d::step2(float dt){
	 vec2f Vnew; // new velocity at time t + dt
	 vec2f Snew; // new position at time t + dt
	 vec2f k1, k2;
	 static float time = 0;
	 time += dt;
	 F = (T - (C * V));
	 A = F / M;
	 k1 = dt * A;
	 F = (T - (C * (V + k1)));
	 A = F / M;
	 k2 = dt * A;
	 // Calculate the new velocity at time t + dt
	 // where V is the velocity at time t
	 Vnew = V + (k1 + k2) / 2;
	 // Calculate the new displacement at time t + dt
	 // where S is the displacement at time t
	 Snew = S + Vnew * dt;
	 // Update old velocity and displacement with the new ones
	 V = Vnew;
	 S = Snew;
 }

 void Sprite2d::step3(float dt){
	 vec2f Vnew; // new velocity at time t + dt
	 vec2f Snew; // new position at time t + dt
	 vec2f k1, k2, k3, k4;
	 static float time = 0;
	 time += dt;

	 
	 F = (T - (C * V));
	 A = F / M;
	 k1 = dt * A;
	 F = (T - (C * (V + k1 / 2)));
	 A = F / M;
	 k2 = dt * A;
	 F = (T - (C * (V + k2 / 2)));
	 A = F / M;
	 k3 = dt * A;
	 F = (T - (C * (V + k3)));
	 A = F / M;
	 k4 = dt * A;
	 // Calculate the new velocity at time t + dt
	 // where V is the velocity at time t
	 Vnew = V + (k1 + 2 * k2 + 2 * k3 + k4) / 6;
	 // Calculate the new displacement at time t + dt
	 // where S is the displacement at time t
	 Snew = S + Vnew * dt;
	 // Update old velocity and displacement with the new ones
	 V = Vnew;
	 S = Snew;
	 
 }

void Sprite2d::stepSimulation(float w, float h,float dt){
	screenDim = vec2f(w, h);
	//assume dt = 10ms means uniform time interval
	dt = deltaT;  //ms
	switch (solve_type)
	{
	case Sprite2d::Euler:
		step0(dt);
		break;
	case Sprite2d::IMPROVED_EULER:
		step1(dt);
		break;
	case Sprite2d::RK2:
		step2(dt);
		break;
	case Sprite2d::RK4:
		step3(dt);
		break;
	default:
		break;
	}
	
	pixelPosition = S;

	//ndc viewport range [-1,1] .ie 2 in screen for every dimensions
	ndcSize = vec2f(pixelSize.x*2.0f / screenDim.x, pixelSize.y*2.0f / screenDim.y);
	ndcPosition = screenToNdc(pixelPosition);
	scale = ndcSize / 2.0;
	translation = ndcPosition;
}

void Sprite2d::run(float w, float h) {
	static float dt = 0;
	stepSimulation(w, h, dt);
    glViewport(0, 0, w, h);
	for (size_t i = 0; i < MAX_PARTICLES; i++)
	{
		vec2f offset = 2.0/screenDim*(particles_offset[i]);
		offset.y *= -1;
		float temp = offset.y;
		//offset.y = sin(temp)*MB::length(T)*0.2;
		temp = offset.x;
		offset.x = 0;// cos(temp * 10) * 0.5;
		drawSprite(w, h, offset);
	}
	
    updateUI(w, h);
}
