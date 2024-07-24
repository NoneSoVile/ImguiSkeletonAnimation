#include "GaussianFilterShaderTester.h"
#include "Shader.h"
#include <numeric>
#include "Matrix.h"
#include "Vector.h"
#include "FileConfig.h"
#include "Common.h"

using MB::matrix4f;
using MB::vec3f;
using MB::vec2f;

void GaussianFilterShaderTester::init() {
    /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/basic.vert");
    string fragShaderFile = resourceFolder + std::string("shaders/basic.frag");

    renderShader = std::make_shared<Shader>();
    renderShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str());//geometryShaderFile.c_str()
    if (!renderShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        vertexShaderFile.c_str(), renderShader->program);
    }

    //======compute shader type================
    string computeShaderFile = resourceFolder + std::string("shaders/gaussianBlur.comp");
    blurShader = std::make_shared<Shader>();
    blurShader->setComputeShader(computeShaderFile.c_str());
    if (!blurShader->isValid()) {
        printf("failed to create shader: %s\n", computeShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", 
        computeShaderFile.c_str(), blurShader->program);
    }

    /*Init data that one of the shaders needs.  The form of data could be a texture,
    a ssbo buffer, etc. Usually we can classify the data into two types. One is the input, 
    the other is the output.
    */
    Size_X = 256;
    Size_Y = 1;
    Size_Z = 1;
    if (shaderType == 0)
        loadAssets0();
    else if(shaderType == 1)
        loadAssets1();

    //kernel data
    int kernelSize = 71;
    kernel_W = kernelSize;
    kernel_H = kernelSize;
    int bufferSize = kernel_W * kernel_H;
    kernelBuffer = new float[bufferSize];


    float sum = 0.0;
    float sigma = 13.5;
    int size = kernelSize;
    for (int x = -size/2; x <= size/2; x++) {
        for (int y = -size/2; y <= size/2; y++) {
            kernelBuffer[(x + size/2)*size + y + size/2] = exp(-(x*x + y*y) / (2*sigma*sigma)) / (2*PI*sigma*sigma);
            sum += kernelBuffer[(x + size/2)*size + y + size/2];
        }
    }


// Normalize the kernel

   for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            kernelBuffer[x*size + y] /= sum;
        }
    }

    //init kernel
    SetupSSBO(Filter_Kernel_Buffer, kernelBuffer, sizeof(float), bufferSize);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * bufferSize, kernelBuffer, GL_STATIC_COPY);
    blurShader->Use(0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, Filter_Kernel_Buffer);
    blurShader->setUniform1i("kernel_W", kernel_W);
    blurShader->setUniform1i("kernel_H", kernel_H);

    //load textures
    loadTexture();
}


void GaussianFilterShaderTester::loadTexture()
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
    srcImgWidth = w;
    srcImgHeight = h;
	SetupTextureData(ourTexture, w, h, img.data);
	printf("Created a new texture with id = %d h = %d, w = %d\n", ourTexture, h, w);
	//SetupTexture(writeTexture, w, h, nullptr);
}

void GaussianFilterShaderTester::loadAssets0(){
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

void GaussianFilterShaderTester::loadAssets1() {
    
}

void GaussianFilterShaderTester::loadRenderState(){
	Config fileConfig;
    fileConfig.LoadConfig("resource/imgui_gl_show.txt");

    
}

void GaussianFilterShaderTester::renderFrame0(int w, int h){
    doCompute();
	matrix4f model;

	CHECK_GL;
    renderShader->Use();
    renderShader->UseAndBindTexture("ourTexture", writeTexture);
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
}

void GaussianFilterShaderTester::renderFrame1(int w, int h){
    matrix4f model;

   
}

void GaussianFilterShaderTester::doCompute(){
    int dim_x = 16;
	int dim_y = 16;
	static float time = 0.0;
	glBindTexture(GL_TEXTURE_2D, 0);
	blurShader->Use(0);CHECK_GL;
    blurShader->setUniform1i("kernel_W", kernel_W);CHECK_GL;
    blurShader->setUniform1i("kernel_H", kernel_H);CHECK_GL;
	//blurShader->setUniform1f("time", time);
	time += 0.01;
	//computeShader[threadid]->setUniform1i("threadid", threadid);
	SetupTexture(writeTexture, srcImgWidth, srcImgHeight);
    glBindImageTexture(1, ourTexture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI); CHECK_GL;
	glBindImageTexture(2, writeTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI); CHECK_GL;
    glDispatchCompute((srcImgWidth + dim_x - 1) / dim_x, (srcImgHeight + dim_y - 1) / dim_y, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT|GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}

void GaussianFilterShaderTester::run(float w, float h) {
    LoadConfig("");
    loadRenderState();
    glViewport(0, 0, w, h);
    if(shaderType == 0)
	    renderFrame0(w, h);
    else if(shaderType == 1){
        renderFrame1(w, h);
    }
}
