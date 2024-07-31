#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];



out highp vec2 vTexCoord;
out highp vec4 vPosition;
out highp vec4 vPositionMV;
out highp vec3 vNormal;
out highp vec3 vNormalMV;
out VS_OUT {
    vec3 normal;
    vec2 vTexCoord;
} vs_out;

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;
        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);
        totalPosition += localPosition * weights[i];
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;

    vec4 position4 = vec4(pos, 1.0f);
    vec4 p = viewModel*position4;
    vPosition = position4;
    vNormal = norm;
    vs_out.normal = norm;
    vs_out.vTexCoord = tex;
    vTexCoord = tex;
    vPositionMV = p;
    mat4 inverse_mv = transpose(inverse(viewModel));
    vec3 n = (inverse_mv*vec4(vNormal, 0.0)).xyz;
    vNormalMV = normalize(n);
}
