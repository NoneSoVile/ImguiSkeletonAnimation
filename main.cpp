#include <iostream>
#include "Keyboard.h"
#include "ShaderTester.h"
#include "GeometryShaderTester.h"
#include "GaussianFilterShaderTester.h"
#include "PhysicsCurve.h"
#include "Sprite2d.h"
#include "Mesh3d.h"
#include "WaterShader.h"
#include "MatTester.h"
#include "SkeletonAnimation.h"
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
using namespace cv;
using namespace std;
using cv::COLOR_RGB2BGRA;

#define WINDOW_WIDTH 2300
#define WINDOW_HEIGHT 1360

#define VIDEO_WIDTH 1280
#define VIDEO_HEIGHT 960



static void glfw_key(GLFWwindow* window, int32_t k, int32_t s, int32_t action, int32_t mods);
static void glfw_mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods);
static void glfw_motion(GLFWwindow* window, double x, double y);
void prepareKeyCodeRemap();
void initScene();
float screenW = WINDOW_WIDTH;
float screenH = WINDOW_HEIGHT;
float startX =  0;
float startY =  0;

#include <cmath>
#define M_PI 3.141592

bool runTester = false;
bool drawTester = true;

ShaderTester* shaderTesterObj = nullptr;

// Main code
int main(int, char**)
{
	prepareKeyCodeRemap();
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(screenW, screenH, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	glewExperimental = GL_TRUE;
	glewInit();

	glfwSetMouseButtonCallback(window, glfw_mouse);
	glfwSetCursorPosCallback(window, glfw_motion);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCanvasResizeCallback("#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);
	//io.Fonts->AddFontDefault();
	io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 38.0f);


	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


	
	ShaderTester shaderTester;
	GeometryShaderTester geoShaderTester;
	GaussianFilterShaderTester gaussianFilterTester;
	PhysicsCurve curve;
	SkeletonAnimation sprite;
	//Mesh3d sprite;
	//WaterShader sprite;
	//shaderTester.init();
	//geoShaderTester.init();
	//gaussianFilterTester.init();
	//curve.init();
	sprite.init(screenW, screenH);

	shaderTesterObj = &sprite;

	// Main loop
#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//curve.run(screenW / 2, screenH);
		sprite.run(screenW , screenH);
		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initScene() {
	
}

static std::map<int32_t, Key::Enum> sKeyCodeRemap;


static void glfw_key(GLFWwindow* window, int32_t k, int32_t s, int32_t action, int32_t mods)
{
	bool handled = false;

	Key::Enum nvkey = Key::K_UNKNOWN;
	std::map<int32_t, Key::Enum>::const_iterator remap = sKeyCodeRemap.find(k);
	if (remap != sKeyCodeRemap.end())
		nvkey = remap->second;

	if (nvkey == Key::K_UNKNOWN)
		return;


	KeyActionType::Enum nvact;
	if (action == GLFW_PRESS) {
		nvact = KeyActionType::DOWN;
		//printf("KeyActionType::DOWN  %c\n", nvkey);
	}
	else if (action == GLFW_REPEAT) {
		nvact = KeyActionType::REPEAT;
		//printf("KeyActionType::GLFW_REPEAT  %c\n", nvkey);
	}
	else {
		nvact = KeyActionType::UP;
		printf("KeyActionType::UP  %c\n", nvkey);
	}
	static int movestepX = 0;
	static int movestepY = 0;
	static float radius = 0.5f;

	if (action == KeyActionType::DOWN ||
		action == KeyActionType::REPEAT) {
		switch (nvkey)
		{
			case 'W':
			{

			}
			break;
			case 'R':
			{
				runTester = !runTester;
			}

			break;
			case 'S': {

			}

			
				break;
			case 'A': {

			}

				break;
			case 'D': {
				drawTester = !drawTester;
				break;
			}		
			case 'T': {
				
				break;
			}
			case 'Y': {

			}

				break;
			}
	}





}


void prepareKeyCodeRemap() {
	sKeyCodeRemap[GLFW_KEY_UNKNOWN] = Key::K_UNKNOWN;
	sKeyCodeRemap[GLFW_KEY_SPACE] = Key::K_SPACE;
	sKeyCodeRemap[GLFW_KEY_APOSTROPHE] = Key::K_APOSTROPHE;
	sKeyCodeRemap[GLFW_KEY_COMMA] = Key::K_COMMA;
	sKeyCodeRemap[GLFW_KEY_PERIOD] = Key::K_PERIOD;
	sKeyCodeRemap[GLFW_KEY_MINUS] = Key::K_MINUS;
	for (int32_t i = 0; i <= 9; i++)
		sKeyCodeRemap[GLFW_KEY_0 + i] = (Key::Enum)(Key::K_0 + (uint32_t)i); // these happen to be same values...
	sKeyCodeRemap[GLFW_KEY_SEMICOLON] = Key::K_SEMICOLON;
	sKeyCodeRemap[GLFW_KEY_EQUAL] = Key::K_EQUAL;
	for (int32_t i = 0; i < 26; i++)
		sKeyCodeRemap[GLFW_KEY_A + i] = (Key::Enum)(Key::K_A + (uint32_t)i); // these happen to be same values...
	sKeyCodeRemap[GLFW_KEY_LEFT_BRACKET] = Key::K_LBRACKET;
	sKeyCodeRemap[GLFW_KEY_BACKSLASH] = Key::K_BACKSLASH;
	sKeyCodeRemap[GLFW_KEY_RIGHT_BRACKET] = Key::K_RBRACKET;
	sKeyCodeRemap[GLFW_KEY_GRAVE_ACCENT] = Key::K_ACCENT_GRAVE;
	sKeyCodeRemap[GLFW_KEY_ESCAPE] = Key::K_ESC;
	sKeyCodeRemap[GLFW_KEY_ENTER] = Key::K_ENTER;
	sKeyCodeRemap[GLFW_KEY_TAB] = Key::K_TAB;
	sKeyCodeRemap[GLFW_KEY_BACKSPACE] = Key::K_BACKSPACE;
	sKeyCodeRemap[GLFW_KEY_INSERT] = Key::K_INSERT;
	sKeyCodeRemap[GLFW_KEY_DELETE] = Key::K_DELETE;
	sKeyCodeRemap[GLFW_KEY_RIGHT] = Key::K_ARROW_RIGHT;
	sKeyCodeRemap[GLFW_KEY_LEFT] = Key::K_ARROW_LEFT;
	sKeyCodeRemap[GLFW_KEY_DOWN] = Key::K_ARROW_DOWN;
	sKeyCodeRemap[GLFW_KEY_UP] = Key::K_ARROW_UP;
	sKeyCodeRemap[GLFW_KEY_PAGE_UP] = Key::K_PAGE_UP;
	sKeyCodeRemap[GLFW_KEY_PAGE_DOWN] = Key::K_PAGE_DOWN;
	sKeyCodeRemap[GLFW_KEY_HOME] = Key::K_HOME;
	sKeyCodeRemap[GLFW_KEY_END] = Key::K_END;
	sKeyCodeRemap[GLFW_KEY_CAPS_LOCK] = Key::K_CAPS_LOCK;
	sKeyCodeRemap[GLFW_KEY_SCROLL_LOCK] = Key::K_SCROLL_LOCK;
	sKeyCodeRemap[GLFW_KEY_NUM_LOCK] = Key::K_NUMLOCK;
	sKeyCodeRemap[GLFW_KEY_PRINT_SCREEN] = Key::K_PRINT_SCREEN;
	sKeyCodeRemap[GLFW_KEY_PAUSE] = Key::K_PAUSE;
	for (int32_t i = 0; i < 12; i++) // we only support through F12 atm. !!!TBD
		sKeyCodeRemap[GLFW_KEY_F1 + i] = (Key::Enum)(Key::K_F1 + (uint32_t)i); // these happen to be same values...
	for (int32_t i = 0; i <= 9; i++)
		sKeyCodeRemap[GLFW_KEY_KP_0 + i] = (Key::Enum)(Key::K_KP_0 + (uint32_t)i); // these happen to be same values...
	sKeyCodeRemap[GLFW_KEY_KP_DECIMAL] = Key::K_KP_DECIMAL;
	sKeyCodeRemap[GLFW_KEY_KP_DIVIDE] = Key::K_KP_DIVIDE;
	sKeyCodeRemap[GLFW_KEY_KP_MULTIPLY] = Key::K_KP_MULTIPLY;
	sKeyCodeRemap[GLFW_KEY_KP_SUBTRACT] = Key::K_KP_SUBTRACT;
	sKeyCodeRemap[GLFW_KEY_KP_ADD] = Key::K_KP_ADD;
	sKeyCodeRemap[GLFW_KEY_KP_ENTER] = Key::K_KP_ENTER;
	sKeyCodeRemap[GLFW_KEY_KP_EQUAL] = Key::K_KP_ENTER; // !!!!TBD
	sKeyCodeRemap[GLFW_KEY_LEFT_SHIFT] = Key::K_SHIFT_LEFT;
	sKeyCodeRemap[GLFW_KEY_LEFT_CONTROL] = Key::K_CTRL_LEFT;
	sKeyCodeRemap[GLFW_KEY_LEFT_ALT] = Key::K_ALT_LEFT;
	sKeyCodeRemap[GLFW_KEY_LEFT_SUPER] = Key::K_WINDOWS_LEFT;
	sKeyCodeRemap[GLFW_KEY_RIGHT_SHIFT] = Key::K_SHIFT_RIGHT;
	sKeyCodeRemap[GLFW_KEY_RIGHT_CONTROL] = Key::K_CTRL_RIGHT;
	sKeyCodeRemap[GLFW_KEY_RIGHT_ALT] = Key::K_ALT_RIGHT;
	sKeyCodeRemap[GLFW_KEY_RIGHT_SUPER] = Key::K_WINDOWS_RIGHT;
	sKeyCodeRemap[GLFW_KEY_MENU] = Key::K_CONTEXT; // !!!TBD proper match?
}




bool resetMouse = false;
static int32_t s_lastButton = 0;
float prevTouchX, prevTouchY;
float firstTouchX, firstTouchY;
float firstTouchTime;
bool bScrolling = false;
bool isXScrolling = false;
int lastState = GLFW_RELEASE;
#define SCROLL_THRESHOLD 50
static void glfw_motion(GLFWwindow* window, double x, double y)
{
	PointerEvent p;
	p.m_x = (float)x;
	p.m_y = (float)y;
	float screenX = x;
	float screenY = y;
	p.m_device = InputDeviceType::MOUSE;
	int lBState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if (lBState == GLFW_RELEASE) {
		double curTime = getCurrentTimeSeconds();
		float deltaTime = curTime - firstTouchTime;
		//LOGI("deltaTime = %f  mFirstTouchTime = %f\n", deltaTime, firstTouchTime);
		float deltaX = screenX - firstTouchX;
		float deltaY = screenY - firstTouchY;
		float dy = -deltaY * .002f / deltaTime;
		float dx = deltaX * .001f / deltaTime;
		//LOGI("ACTION_UP velocity vx = %f  vy = %f\n", dx, dy);

		if (bScrolling) {
			//onFling(dx, dy);
		}
		lastState = GLFW_RELEASE;

	}
	else if (lBState == GLFW_PRESS) {
		if (lastState == GLFW_RELEASE) {
			prevTouchX = firstTouchX = screenX;
			prevTouchY = firstTouchY = screenY;
			firstTouchTime = getCurrentTimeSeconds();

			bScrolling = false;
			LOGI("ACTION_DOWN velocity \n");
		}
		else if(lBState == GLFW_PRESS){
			if (!bScrolling) {
				float deltaX = abs(screenX - firstTouchX);
				float deltaY = abs(screenY - firstTouchY);
				bScrolling = (deltaX >= SCROLL_THRESHOLD) || (deltaY >= SCROLL_THRESHOLD);
				if (!bScrolling) {
					LOGI("ACTION_MOVE not scrolling\n");
					prevTouchX = screenX;
					prevTouchY = screenY;

					return;
				}
				else {
					LOGI("ACTION_MOVE scrolling started!!========\n");
					isXScrolling = deltaX > deltaY ? true : false;
				}
			}
			else {
				LOGI("ACTION_MOVE is scrolling-------------->>>>\n");

			}


			float deltaX = screenX - prevTouchX;
			float deltaY = screenY - prevTouchY;
			float dx = deltaX ;
			float dy = -deltaY ;

			if (bScrolling) {
				/*if (isXScrolling) {
					dy = 0;
				}
				else {
					dx = 0;
				}*/
				if (shaderTesterObj)
				{
					shaderTesterObj->onScroll(dx, dy);
				}
			}


			else {
				LOGI("Ignore touch event action : mouse move\n");
			}
			prevTouchX = screenX;
			prevTouchY = screenY;
		}
		lastState = GLFW_PRESS;
	}

}

static void glfw_mouse(GLFWwindow* window, int32_t button, int32_t action, int32_t mods)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	PointerEvent p;
	p.m_x = (float)x;
	p.m_y = (float)y;
	float screenX = x;
	float screenY = y;
	
	//if(action == GLUT_UP)
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
		

    }
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        p.m_id = MouseButton::MIDDLE;
        //printf("GLFW_MOUSE_BUTTON_MIDDLE\n");
    }

    else {
        p.m_id = MouseButton::RIGHT;
        //printf("GLFW_MOUSE_BUTTON_RIGHT\n");
    }
	printf("mouse x = %d, y = %d\n", (int)x, (int)y);

	//s_lastButton = p.m_id;
	//if(sc)
}