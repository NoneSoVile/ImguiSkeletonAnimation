#version 310 es
precision highp float;
in highp vec2 vTexCoord;
in highp vec4 vPosition;
in highp vec4 vPositionMV;
in highp vec3 vNormal;
in highp vec3 vNormalMV;

out highp vec4 color;

#define MAX_LIGHTS 20
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mediump float specularPower;
uniform mediump float diffusePower;
uniform mediump int lightNum;
uniform mediump vec3 lights[MAX_LIGHTS];
uniform mediump vec3 lights_Color[MAX_LIGHTS];

uniform sampler2D ourTexture;
uniform int useTexture;
uniform float alphaColor;

const float PI = 3.1415926;

vec3 brightnessContrast(vec3 value, float brightness, float contrast)
{
    return (value - 0.5) * contrast + 0.5 + brightness;
}

void main() {
    vec3 intense = vec3(1.0, 1.0, 1.0);
    vec3 pv = vec3(0.0, 0.0, 0.0);
    float m = specularPower;

    vec3 l[5];
    vec3 EL[5];
    vec3 Kd = diffusePower/float(lightNum)*lights_Color[0]/PI;
    vec3 Ks = 6.0/float(lightNum)*lights_Color[0]*(m + 8.0)/(8.0*PI);
    vec4 p = vPositionMV;

    float intensity = 1.0;
    vec3 n = vNormalMV;

    vec3 v = normalize(pv - p.xyz);
    intense = vec3(0.0, 0.0, 0.0);
    vec3 intenseOfMainLight = vec3(0.0, 0.0, 0.0);

    for(int k = 0; k < lightNum; k++){
        float theta = float(k)* 2.0*PI/ float(lightNum);
        l[k] = (vec4(lights[k], 1.0)).xyz;
        l[k] = (view*vec4(l[k], 1.0)).xyz;
        float distance = length(l[k] - p.xyz);
        l[k] = normalize(l[k] - p.xyz);
        EL[k] = vec3(1.0);
        vec3 h = normalize(v + l[k]);
        float cosTh = max(dot(n, h), 0.0);
        float cosTi = max(dot(n, l[k]), 0.0);
        float coslh = max(dot(h, l[k]), 0.0);
        if(k == 0)
            intenseOfMainLight = (Kd + Ks*pow(cosTh, m)) * EL[k]*cosTi*(lights_Color[k])/ ( 1.0);
        else
            intense += (Kd + Ks*pow(cosTh, m)) * EL[k]*cosTi*(lights_Color[k])/ ( 1.0);
    }

     vec4 finalColor = texture(ourTexture, vTexCoord);
     if(useTexture == 0){
        finalColor = vec4(lights_Color[0], 1.0);
     }
     
     finalColor.xyz = finalColor.xyz*(intense + intenseOfMainLight);
     color = finalColor * vec4(vec3(1.0, 1.0, 1.0), alphaColor);
 }
