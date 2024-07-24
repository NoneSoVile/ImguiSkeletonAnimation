#version 330 core
precision highp float;
layout(location = 0) in vec3 pos_width;
//layout(location = 1) in vec4 pos_col;
uniform float lineWidth = 0.1;
uniform mat4 model;
out vec4 v_col;
out  float v_line_width;
void main()
{
    v_col = vec4(1.0);//pos_col;
    v_line_width = lineWidth;//pos_width.w;
    gl_Position = vec4(pos_width.xyz, 1.0);
    gl_Position = model*vec4(pos_width.x, pos_width.y, pos_width.z,  1.0);
    //gl_Position.w = 1.0;
};