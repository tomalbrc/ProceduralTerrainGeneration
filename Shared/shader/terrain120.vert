#version 120
#extension GL_EXT_gpu_shader4 : require

varying vec4 vertexWorldPos;
varying vec3 fragNormal;

uniform float time;
uniform vec2 chunkSize;
uniform float quadScale;

void main() {
    gl_Position = ftransform();
    
    // pass through
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
