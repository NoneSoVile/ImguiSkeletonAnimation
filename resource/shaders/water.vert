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

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;
uniform highp mat4 uProjection;
uniform highp mat4 u_inv_MV;
uniform highp float u_time;

#define MAX_WAVES 20
uniform int waveCount;
uniform int wavePerturbation;
uniform vec2 waves_D[MAX_WAVES];
uniform vec3 waves_AWP[MAX_WAVES];
uniform float time;
uniform float waves_Power;
const float PI = 3.1415926;


float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in vec2 p ) {
    vec2 i = floor( p );
    vec2 f = fract( p );	
	vec2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), 
                     hash( i + vec2(1.0,0.0) ), u.x),
                mix( hash( i + vec2(0.0,1.0) ), 
                     hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

float sea_octave(vec2 uv, float choppy) {
    uv += noise(uv);   
    vec2 wv = 1.0-abs(sin(uv));
    vec2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

void main() {
    mat4 MVP = projection*view*model;
    mat4 MV = view*model;
    

    vec4 position4 = vec4(position, 1.0f);
    //water height of vertices calculation

    float dpdx = 0.0;
    float dpdz = 0.0;
    if(waves_Power <= 1.01){
        for(int i = 0; i < waveCount; i++){
            vec3 AWP = waves_AWP[i];
            vec2 D = waves_D[i];
            float A = AWP.x*1.0;
            float W = AWP.y*130.0;
            float P = AWP.z*10.0;
            vec2 posXZ = position4.xz;
            position4.y += A * sin(dot(D, posXZ) * W + time * P);
            dpdx += W * D.x* A* cos(dot(D, posXZ) * W + time * P);
            dpdz += W * D.y* A* cos(dot(D, posXZ) * W + time * P);
        }
    }else{
        for(int i = 0; i < waveCount; i++){
            vec3 AWP = waves_AWP[i];
            vec2 D = waves_D[i];
            float A = AWP.x*1.0;
            float W = AWP.y*130.0;
            float P = AWP.z*10.0;
            vec2 posXZ = position4.xz;
            position4.y += 2.0*A * pow(0.5*(sin(dot(D, posXZ) * W + time * P) + 1.0), waves_Power);
            dpdx += waves_Power * W * D.x* A* cos(dot(D, posXZ) * W + time * P) * pow(0.5*(sin(dot(D, posXZ) * W + time * P) + 1.0), waves_Power - 1.0);
            dpdz += waves_Power * W * D.y* A* cos(dot(D, posXZ) * W + time * P) * pow(0.5*(sin(dot(D, posXZ) * W + time * P) + 1.0), waves_Power - 1.0);
        }
    }

    //pertrubation
    float h = 0.0;  
    if(wavePerturbation > 0){
        float freq_scale = 2.0;
        float amp_scale = 0.2;
        float amp = 5.0;
        float freq = 1.0;
        float choppy = 30.0;
        float d = 0.0;
  
        for(int i = 0; i < 20; i++){
            d = sea_octave((position4.xz + time)*freq, choppy);
            d += sea_octave((position4.xz - time)*freq, choppy);
            h += d * amp;  
            
            choppy = mix(choppy,1.0,0.2);
            amp *= amp_scale;
            freq *= freq_scale;
        }
    }

    position4.y += h;
        


    
    vPosition = position4;
    vNormal = vec3(-dpdx, 1.0, -dpdz);//normal;
    vTexCoord = texCoord;

    vec4 p = MV*position4;
    mat4 inverse_mv = transpose(inverse(MV));
    vec3 n = (inverse_mv*vec4(vNormal, 0.0)).xyz;//vec3(0.0, 0.0, 1.0);
    vPositionMV = p;
    vNormalMV = n = normalize(n);
    gl_Position = MVP*position4;
}