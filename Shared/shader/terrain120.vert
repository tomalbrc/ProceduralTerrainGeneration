#version 120
#extension GL_EXT_gpu_shader4 : require

flat varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;

uniform float time;
uniform vec2 chunkSize;
uniform float quadScale;

void main() {
    gl_Position = ftransform();
    // water
    if (gl_Vertex.y < 7.f) gl_Position.y += ((sin(((mod(gl_Vertex.x+gl_Vertex.z,(quadScale*(chunkSize.x-1))))+time)/200.f)-0.5f));

    // pass through
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord0;
    gl_TexCoord[2] = gl_MultiTexCoord0;
    gl_TexCoord[3] = gl_MultiTexCoord0;
}
