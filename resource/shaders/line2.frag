#version 330 core
precision highp float;
uniform vec2 u_aa_radius;
in vec4 g_col;
in float g_u;
in float g_v;
in float g_line_width;
in float g_line_length;
out vec4 frag_color;
void main()
{
    float au = 1.0 - smoothstep( 1.0 - ((2.0*u_aa_radius[0]) / g_line_width),  1.0, abs(g_u / g_line_width) );
    float av = 1.0 - smoothstep( 1.0 - ((2.0*u_aa_radius[1]) / g_line_length), 1.0, abs(g_v / g_line_length) );
    frag_color = g_col;
    frag_color.a *= au;
};