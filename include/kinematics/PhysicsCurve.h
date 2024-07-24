#pragma once
#include "Common.h"
#include "ShaderTester.h"
#include "RenderState.h"
#include "CubicSpline.h"
#include "NvMath.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include "imgui.h"
#include <string>
using std::string;
using MB::vec2f;
using MB::matrix4f;
using MB::matrix3f;
using MB::quaternionf;
class Shader;
#define MAX_RESULT_LEN 10

class RigidBody2D {
public:
    float fMass; // total mass (constant)
    float fInertia; // mass moment of inertia
    float fInertiaInverse; // inverse of mass moment of inertia
    vec3f vPosition; // position in earth coordinates
    vec3f vVelocity; // velocity in earth coordinates
    vec3f vVelocityBody; // velocity in body coordinates
    vec3f vAngularVelocity; // angular velocity in body coordinates
    float fSpeed; // speed
    float fOrientation; // orientation
    vec3f vForces; // total force on body
    vec3f vMoment; // total moment on body
    float ThrustForce; // Magnitude of the thrust force
    vec3f PThrust, SThrust; // bow thruster forces
    float fWidth; // bounding dimensions
    float fLength;
    float fHeight;
    vec3f CD; // location of center of drag in body coordinates
    vec3f CT; // location of center of propeller thrust in body coords.
    vec3f CPT; // location of port bow thruster thrust in body coords.
    vec3f CST; // location of starboard bow thruster thrust in body
    // coords.
    float ProjectedArea; // projected area of the body
    RigidBody2D(void);
    void CalcLoads(void);
    void UpdateBodyEuler(double dt);
    void SetThrusters(bool p, bool s);
    void ModulateThrust(bool up);
};

typedef struct _RigidBody {
    float fMass; // total mass
    matrix3f mInertia; // mass moment of inertia
    // in body coordinates
    matrix3f mInertiaInverse; // inverse of mass moment of inertia
    vec2f vPosition; // position in earth coordinates
    vec2f vVelocity; // velocity in earth coordinates
    vec2f vVelocityBody; // velocity in body coordinates
    vec2f vAngularVelocity;// angular velocity in body coordinates
    vec2f vEulerAngles; // Euler angles in body coordinates
    float fSpeed; // speed (magnitude of the velocity)
    quaternionf qOrientation; // orientation in earth coordinates
    vec2f vForces; // total force on body
    vec2f vMoments; // total moment (torque) on body
} RigidBody, * pRigidBody;

struct particle {

    //physics property
    vec2f position;
    vec2f velocity;
    vec2f acceleration;

    //initial properties
    vec2f position0;
    vec2f velocity0;
    vec2f acceleration0;



    //
    vec2f getPositionByTime(float t) {
        vec2f curPos;
        curPos.x = 0.5 * t + position0.x;
        curPos.y = 0.5 * t * t + 2*t + position0.y;


        return curPos;
    }

    vec2f getVelocityByTime(float t) {
        vec2f curV;
        curV.x = 0.5;
        curV.y = t + 2;


        return curV;
    }

    vec2f getAccelerationByTime(float t) {
        vec2f acc;
        acc.x = 0;
        acc.y = 1;


        return acc;
    }

    vec2f getVelocityByPosition(vec2f pos) {
        vec2f curV;
        curV.x = std::sqrt(2 * acceleration.x * (pos.x - position0.x));
        curV.y = std::sqrt(2 * acceleration.y * (pos.y - position0.y));

        return curV;
    }

    vec2f getPositionByVelocity(vec2f vel) {
        vec2f curPos;
        curPos.x = 0.5 * (vel.x * vel.x - velocity0.x * velocity0.x) / acceleration.x + position0.x;
        curPos.y = 0.5 * (vel.y * vel.y - velocity0.y * velocity0.y) / acceleration.y + position0.y;

        return curPos;
    }

    vec2f getAccelerationByVelocity(vec2f vel) {
        vec2f acc;
        acc.x = (vel.x * vel.x - velocity0.x * velocity0.x) / (2 * (position.x - position0.x));
        acc.y = (vel.y * vel.y - velocity0.y * velocity0.y) / (2 * (position.y - position0.y));

        return acc;
    }

    void update(float time) {
        position = getPositionByTime(time);
        velocity = getVelocityByTime(time);
        acceleration = getAccelerationByTime(time);
        // Update other properties as needed
    }

public:

};

class PhysicsCurve : public ShaderTester

{
protected:
    int shaderType = 1;
    std::shared_ptr<Shader> geometryShader;
    GLuint VBO;
	GLuint VAO;
	GLuint EBO;

    //render states
    float zRotation = 0;
    int usingGeoShader = 0;
    float lineWidth = 0.1;
    int enableGLMultisample = 0;
    //0: GL_TRIANGLES   1:  GL_LINE_STRIP_ADJACENCY  2:GL_POINTS  
    int drawMode = 0;

    std::vector<vec3f> pointsOfCurve;

    //GUI
    bool show_demo_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImVec4 clear_pos = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    void loadAssets0();
    void loadAssets1();
    void loadRenderState();
    void generateTrackPoints();
    void generateTrackPointsByCubicSpline();
    void generateTrackPointsByBeZierCurve();
    void generateTrackPointsByBSpline();
    void renderFrame0(int w, int h);
    void renderFrame1(int w, int h);
    void updateUI(int w, int h);
public:
    void init();
    void run(float w, float h);
};

