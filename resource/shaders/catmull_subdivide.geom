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

    // Compute face point
    vec4 facePoint = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;

    // Compute new vertex positions
    vec4 newVert1 = (gl_in[0].gl_Position + midPoint1 + midPoint3 + facePoint) / 4.0;
    vec4 newVert2 = (gl_in[1].gl_Position + midPoint1 + midPoint2 + facePoint) / 4.0;
    vec4 newVert3 = (gl_in[2].gl_Position + midPoint2 + midPoint3 + facePoint) / 4.0;

        // Compute midpoints of texture coordinates
    vec2 midTexCoord1 = (gs_in[0].vTexCoord + gs_in[1].vTexCoord) * 0.5;
    vec2 midTexCoord2 = (gs_in[1].vTexCoord + gs_in[2].vTexCoord) * 0.5;
    vec2 midTexCoord3 = (gs_in[2].vTexCoord + gs_in[0].vTexCoord) * 0.5;

    // Compute face texture coordinate
    vec2 faceTexCoord = (gs_in[0].vTexCoord + gs_in[1].vTexCoord + gs_in[2].vTexCoord) / 3.0;

        // Compute new texture coordinate 
    vec2 newTex1 = (gs_in[0].vTexCoord + midTexCoord1 + midTexCoord3 + faceTexCoord) / 4.0;
    vec2 newTex2 = (gs_in[1].vTexCoord + midTexCoord1 + midTexCoord2 + faceTexCoord) / 4.0;
    vec2 newTex3 = (gs_in[2].vTexCoord + midTexCoord2 + midTexCoord3 + faceTexCoord) / 4.0;

    vec3 avgNormal = normalize(gs_in[0].normal + gs_in[1].normal + gs_in[2].normal);

    // Output the first triangle
    mat4 viewModel = view * model;
    mat4 inverse_mv = transpose(inverse(viewModel));
    vec4 curpos = vec4(0.0);

    vec4 p = viewModel*newVert1;   
    vec3 n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = newVert1;
    vTexCoord = newTex1;
    vPositionMV = p;
    gl_Position = newVert1;
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

    p = viewModel*facePoint;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = facePoint;
    vTexCoord = faceTexCoord;
    vPositionMV = p;
    gl_Position = facePoint;
    EmitVertex();
    EndPrimitive();

    // Output the second triangle
    p = viewModel*newVert2;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = newVert2;
    vTexCoord = newTex2;
    vPositionMV = p;
    gl_Position = newVert2;
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

    p = viewModel*facePoint;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = facePoint;
    vTexCoord = faceTexCoord;
    vPositionMV = p;
    gl_Position = facePoint;
    EmitVertex();
    EndPrimitive();

    // Output the third triangle
    p = viewModel*newVert3;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = newVert3;
    vTexCoord = newTex3;
    vPositionMV = p;
    gl_Position = newVert3;
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

    p = viewModel*facePoint;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = facePoint;
    vTexCoord = faceTexCoord;
    vPositionMV = p;
    gl_Position = facePoint;
    EmitVertex();
    EndPrimitive();

    // Output the fourth triangle
    p = viewModel*midPoint1;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = midPoint1;
    vTexCoord = midTexCoord1;
    vPositionMV = p;
    gl_Position = midPoint1;
    EmitVertex();

    p = viewModel*newVert2;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = newVert2;
    vTexCoord = newTex2;
    vPositionMV = p;
    gl_Position = newVert2;
    EmitVertex();

    p = viewModel*newVert1;   
    n = (inverse_mv*vec4(avgNormal, 0.0)).xyz;
    vNormal = avgNormal;
    vNormalMV = normalize(n);
    vPosition = newVert1;
    vTexCoord = newTex1;
    vPositionMV = p;
    gl_Position = newVert1;
    EmitVertex();
    EndPrimitive();
}
