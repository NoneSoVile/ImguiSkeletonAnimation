#version 330 core
precision highp float;
layout(triangles) in;
layout(triangle_strip, max_vertices = 4) out;

uniform float lineWidth = 0.1;
uniform uint usingGeoShader = 0u;

void main() {
    if(usingGeoShader == 0u)
    {
        for(int i = 0; i < 3; i++) { // You used triangles, so it's always 3
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
        return;
    }

    vec2 p0 = gl_in[0].gl_Position.xy;
    vec2 p1 = gl_in[1].gl_Position.xy;
    vec2 p2 = gl_in[2].gl_Position.xy;

    vec2 v0 = normalize(p1 - p0);
    vec2 v1 = normalize(p2 - p1);
    vec2 n0 = vec2(-v0.y, v0.x);
    vec2 n1 = vec2(-v1.y, v1.x);

    vec2 offset0 = n0 * (lineWidth * 0.5);
    vec2 offset1 = n1 * (lineWidth * 0.5);

    gl_Position = vec4(p0 - offset0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1 - offset0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p0 + offset0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1 + offset0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();

    gl_Position = vec4(p1 - offset0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p2 - offset1, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p1 + offset0, 0.0, 1.0);
    EmitVertex();

    gl_Position = vec4(p2 + offset1, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}
