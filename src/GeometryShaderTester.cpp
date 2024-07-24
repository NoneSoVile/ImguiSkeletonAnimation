#include "GeometryShaderTester.h"
#include "Shader.h"
#include <numeric>
#include "Matrix.h"
#include "Vector.h"
#include "FileConfig.h"
#include "BezierCurve.h"
#include "BSpline.h"
#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"


using MB::matrix4f;
using MB::vec3f;
using MB::vec2f;

void GeometryShaderTester::init() {
    /*create shaders
    */
    string vertexShaderFile = resourceFolder + std::string("shaders/line.vert");
    string geometryShaderFile = resourceFolder + std::string("shaders/line.geom");
    string fragShaderFile = resourceFolder + std::string("shaders/line.frag");
    /*string vertexShaderFile = resourceFolder + std::string("shaders/line2.vert");
    string geometryShaderFile = resourceFolder + std::string("shaders/line2.geom");
    string fragShaderFile = resourceFolder + std::string("shaders/line2.frag");*/
    if(shaderType == 1){
        vertexShaderFile = resourceFolder + std::string("shaders/line2.vert");
        geometryShaderFile = resourceFolder + std::string("shaders/line2.geom");
        fragShaderFile = resourceFolder + std::string("shaders/line2.frag");
    }
    geometryShader = std::make_shared<Shader>();
    geometryShader->setShader(vertexShaderFile.c_str(), fragShaderFile.c_str(), geometryShaderFile.c_str());//geometryShaderFile.c_str()
    if (!geometryShader->isValid()) {
        printf("failed to create shader: %s\n", vertexShaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", vertexShaderFile.c_str(), geometryShader->program);
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
}



void GeometryShaderTester::loadAssets0(){
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

void GeometryShaderTester::loadAssets1() {
    //generateTrackPoints();
    //generateTrackPointsByCubicSpline();
    generateTrackPointsByBeZierCurve();
    //generateTrackPointsByBSpline();
    GLfloat* vertices = new GLfloat[pointsOfCurve.size() * 3];

    int count = 0;
    for (std::size_t i = 0; i < pointsOfCurve.size(); i++) {
        if (true) {
            vertices[count * 3] = pointsOfCurve[i].x;
            vertices[count * 3 + 1] = pointsOfCurve[i].y;
            vertices[count * 3 + 2] = pointsOfCurve[i].z;
            count++;
        }
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*count*3, vertices, GL_STATIC_DRAW); CHECK_GL;

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); CHECK_GL;

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0); CHECK_GL;

    // TexCoord attribute
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    //glEnableVertexAttribArray(1); CHECK_GL;

    glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    CHECK_GL;
    delete[] vertices;
}

void GeometryShaderTester::loadRenderState(){
	Config fileConfig;
    fileConfig.LoadConfig("resource/imgui_gl_show.txt");
	zRotation = fileConfig.Read("zRotation", 0.0);
	usingGeoShader = fileConfig.Read("usingGeoShader", 0);
	lineWidth = fileConfig.Read("lineWidth", 0.2);
    enableGLMultisample = fileConfig.Read("enableGLMultisample", 0);
    drawMode = fileConfig.Read("drawMode", drawMode);
    
}

void GeometryShaderTester::generateTrackPoints(){
        /*xyzw  format  */
        GLfloat vertices[] = {
		1.0f, -1.0f, 0.0f,     //RB       from the view direction of -z
		1.0f, 1.0f, 0.0f,       //RT 
		-1.0f, 1.0f, 0.0f,    //LT
		-1.0f, -1.0f, 0.0f,      //LB
	    };
        float baseX = 0.0, baseY = -1.0, baseZ = 0.0;
        float offsetY = 0.01;
        int pointsRequired = 200;
        pointsOfCurve.clear();
        vec3f LPoint(-baseX, baseY, baseZ);
        vec3f RPoint(baseX, baseY, baseZ);
        for (size_t i = 0; i < pointsRequired; i++)
        {          
            pointsOfCurve.push_back(LPoint);
            LPoint.y += offsetY;
            LPoint.x = 0.7*sin(6*LPoint.y);
            //pointsOfCurve.push_back(RPoint);
        }
        
}

void GeometryShaderTester::generateTrackPointsByCubicSpline(){
    GLfloat controlVertices[] = {

    -1.0f, 1.0f, 0.0f,    //LT
    -0.7f, -1.0f, 0.0f,      //LB

    -0.0f, -1.0f, 0.0f,      //Middle Bottom
        0.7f, -1.0f, 0.0f,     //RB       from the view direction of -z
    1.0f, 1.0f, 0.0f,       //RT 
    };

    float baseX = 0.0, baseY = -1.0, baseZ = 0.0;
    float offsetY = 0.01;
    int pointsRequired = 120;
    pointsOfCurve.clear();
    vector<double> xVals, yVals;
    int stride = 3;
    int size =  sizeof(controlVertices) / sizeof(controlVertices[0]);
    int pointSize = size / stride;
    for (size_t i = 0; i < pointSize; i++)
    {
        xVals.emplace_back(controlVertices[i*stride]*0.5);
        yVals.emplace_back(controlVertices[i*stride + 1]*0.5);
    }
    CubicSpline spline(xVals, yVals);
    pointsOfCurve = spline.interpolateAllPoints(pointsRequired);
}

void GeometryShaderTester::generateTrackPointsByBeZierCurve(){
    /*xyzw  format  */
    GLfloat controlVertices[] = {
                -1.0f, -1.0f, 0.0f, // LB
                              1.0f, -1.0f, 0.0f,  // RB       from the view direction of -z

1.0f, 1.0f, 0.0f,   // RT
        -1.0f, 1.0f, 0.0f,  // LT
                                        

 

    };
    float baseX = 0.0, baseY = -1.0, baseZ = 0.0;
    float offsetY = 0.01;
    int pointsRequired = 80;
    pointsOfCurve.clear();

    int stride = 3;
    int size =  sizeof(controlVertices) / sizeof(controlVertices[0]);
    int pointSize = size / stride;
    vector<vec3f> points;
    for (size_t i = 0; i < pointSize; i++)
    {
        vec3f controlPoint(controlVertices[i*stride], controlVertices[i*stride + 1], controlVertices[i*stride + 2]);
        points.push_back(controlPoint*0.5);
    }
    BezierCurve bezier(points);
    pointsOfCurve = bezier.interpolateAllPoints(pointsRequired);
}

void GeometryShaderTester::generateTrackPointsByBSpline() {
    /*xyzw  format  */
    GLfloat controlVertices[] = {
        -2.5f, -2.5f, 0.0f,
          -1.0f, -1.0f, 0.0f, // LB
          1.0f, -1.0f, 0.0f,  // RB       from the view direction of -z


        -1.0f, 1.0f, 0.0f,  // LT
       1.0f, 1.0f, 0.0f,   // RT
        1.5f, 1.5f, 0.0f,
        5.5f, 8.5f, 0.0f,
       
    };
    float baseX = 0.0, baseY = -1.0, baseZ = 0.0;
    float offsetY = 0.01;
    int pointsRequired = 80;
    pointsOfCurve.clear();
    int stride = 3;
    int size = sizeof(controlVertices) / sizeof(controlVertices[0]);
    int pointSize = size / stride;
    vector<vec3f> points;
    int degree = 3; //degree must be less than or equal to point count - 1
    for (size_t i = 0; i < pointSize; i++)
    {
        vec3f controlPoint(controlVertices[i * stride], controlVertices[i * stride + 1], controlVertices[i * stride + 2]);
        points.push_back(controlPoint * 0.5);
    }
    BSpline bSpline;
    pointsOfCurve = bSpline.generatePoints(pointsRequired, degree, points);
}

void GeometryShaderTester::updateUI(int w, int h) {
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowBgAlpha(0.5f);
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Helloxxx, world!", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
        ImGui::SliderFloat3("position", (float*)&clear_pos, 0, 1);
        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }


    //glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
}

void GeometryShaderTester::renderFrame0(int w, int h){

	matrix4f model;
	MB::rotationZ(model, zRotation);
    if (enableGLMultisample)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
	geometryShader->Use(); CHECK_GL;
	geometryShader->Use().setUniformMatrix4fv("model", model._array, 1, GL_FALSE)
		.setUniform1ui("usingGeoShader", usingGeoShader)
							.setUniform1f("lineWidth", lineWidth); CHECK_GL;
	CHECK_GL;
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

void GeometryShaderTester::renderFrame1(int w, int h){
    matrix4f model;
    MB::rotationZ(model, zRotation);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    if (enableGLMultisample)
        glEnable(GL_MULTISAMPLE);
    else
        glDisable(GL_MULTISAMPLE);
    geometryShader->Use().setUniform1ui("usingGeoShader", usingGeoShader)
                        .setUniform2f("u_viewport_size",(float)w,(float)h)
                      .setUniform2f("u_aa_radius",2.f,0.3f)
                    .setUniform1f("lineWidth", lineWidth)
                    .setUniformMatrix4fv("model", model._array, 1, GL_FALSE); CHECK_GL;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); CHECK_GL;
    glEnableVertexAttribArray(0); CHECK_GL;
    GLenum mode = GL_LINE_STRIP_ADJACENCY;
    if (drawMode == 0) {
        mode = GL_TRIANGLES;
    }
    else if (drawMode == 1) {
        mode = GL_LINE_STRIP_ADJACENCY;
    }
    else if (drawMode == 2) {
        mode = GL_LINES_ADJACENCY;
    }
    glDrawArrays(mode, 0, pointsOfCurve.size()); CHECK_GL;
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); CHECK_GL;
	glDisable(GL_BLEND);
}

void GeometryShaderTester::run(float w, float h) {
    LoadConfig("");
    loadRenderState();
    glViewport(0, 0, w, h);
    updateUI(w, h);
    if(shaderType == 0)
	    renderFrame0(w, h);
    else if(shaderType == 1){
        renderFrame1(w, h);
    }
}
