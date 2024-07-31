#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 12) out;

in VS_OUT {
    vec3 normal;
    vec2 vTexCoord;
} gs_in[];

out GS_OUT {
    vec2 vTexCoord;
    vec4 vPosition;
    vec4 vPositionMV;
    vec3 vNormal;
    vec3 vNormalMV;
} gs_out;


uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out  vec2 vTexCoord;
out  vec4 vPosition;
out  vec4 vPositionMV;
out  vec3 vNormal;
out  vec3 vNormalMV;

void main() {
    // Compute edge midpoints
    vec4 midPoint1 = (gl_in[0].gl_Position + gl_in[1].gl_Position) * 0.5;
    vec4 midPoint2 = (gl_in[1].gl_Position + gl_in[2].gl_Position) * 0.5;
    vec4 midPoint3 = (gl_in[2].gl_Position + gl_in[0].gl_Position) * 0.5;


        // Compute midpoints of texture coordinates
    vec2 midTexCoord1 = (gs_in[0].vTexCoord + gs_in[1].vTexCoord) * 0.5;
    vec2 midTexCoord2 = (gs_in[1].vTexCoord + gs_in[2].vTexCoord) * 0.5;
    vec2 midTexCoord3 = (gs_in[2].vTexCoord + gs_in[0].vTexCoord) * 0.5;

    vec3 avgNormal = normalize(gs_in[0].normal + gs_in[1].normal + gs_in[2].normal);

    // Output the first triangle
    mat4 viewModel = view * model;
    mat4 inverse_mv = transpose(inverse(viewModel));
    vec4 curpos = vec4(0.0);

    vec4 p = viewModel*midPoint1;   
    vec3 n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint1;
    vTexCoord = midTexCoord1;
    vPositionMV = p;
    gl_Position = midPoint1;
    EmitVertex();


    p = viewModel*midPoint2;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint2;
    vTexCoord = midTexCoord2;
    vPositionMV = p;
    gl_Position = midPoint2;
    EmitVertex();

    p = viewModel*midPoint3;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint3;
    vTexCoord = midTexCoord3;
    vPositionMV = p;
    gl_Position = midPoint3;
    EmitVertex();
    EndPrimitive();

    // Output the second triangle
    p = viewModel*midPoint1;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint1;
    vTexCoord = midTexCoord1;
    vPositionMV = p;
    gl_Position = midPoint1;
    EmitVertex();

    p = viewModel*gl_in[0].gl_Position;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = gl_in[0].gl_Position;
    vTexCoord = gs_in[0].vTexCoord;
    vPositionMV = p;
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    p = viewModel*midPoint3;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint3;
    vTexCoord = midTexCoord3;
    vPositionMV = p;
    gl_Position = midPoint3;
    EmitVertex();
    EndPrimitive();

    // Output the third triangle
    p = viewModel*midPoint2;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint2;
    vTexCoord = midTexCoord2;
    vPositionMV = p;
    gl_Position = midPoint2;
    EmitVertex();

    p = viewModel*gl_in[1].gl_Position;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = gl_in[1].gl_Position;
    vTexCoord = gs_in[1].vTexCoord;
    vPositionMV = p;
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    p = viewModel*midPoint1;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint1;
    vTexCoord = midTexCoord1;
    vPositionMV = p;
    gl_Position = midPoint1;
    EmitVertex();
    EndPrimitive();

    // Output the fourth triangle
    p = viewModel*midPoint3;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint3;
    vTexCoord = midTexCoord2;
    vPositionMV = p;
    gl_Position = midPoint3;
    EmitVertex();

    p = viewModel*gl_in[2].gl_Position;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = gl_in[2].gl_Position;
    vTexCoord = gs_in[2].vTexCoord;
    vPositionMV = p;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    p = viewModel*midPoint2;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint2;
    vTexCoord = midTexCoord2;
    vPositionMV = p;
    gl_Position = midPoint2;
    EmitVertex();
    EndPrimitive();
}
