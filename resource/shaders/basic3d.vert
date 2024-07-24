#version 310 es
precision highp float;
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
out highp vec2 vTexCoord;
out highp vec4 vPosition;
out highp vec4 vPositionMV;
out highp vec3 vNormal;
out highp vec3 vNormalMV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mediump mat4 uProjection;
uniform mediump mat4 u_inv_MV;
uniform mediump float u_time;

const float PI = 3.1415926;

void main() {
    mat4 MVP = projection*view*model;
    mat4 MV = view*model;
    gl_Position = MVP*vec4(position, 1.0f);

    vec4 position4 = vec4(position, 1.0f);
    vPosition = position4;
    vNormal = normal;
    vTexCoord = texCoord;

    vec4 p = MV*position4;
    mat4 inverse_mv = transpose(inverse(MV));
    vec3 n = (inverse_mv*vec4(normal, 0.0)).xyz;//vec3(0.0, 0.0, 1.0);
    vPositionMV = p;
    vNormalMV = n = normalize(n);
}