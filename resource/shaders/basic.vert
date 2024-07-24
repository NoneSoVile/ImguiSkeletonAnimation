#version 310 es
precision highp float;
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
out vec2 TexCoord;

uniform vec2 translation;
uniform vec2 scale;

void main()
{
    vec2 posxy = position.xy;
    posxy = scale*posxy + translation;
    gl_Position = vec4(posxy, 0.0, 1.0);
    TexCoord = texCoord;
}