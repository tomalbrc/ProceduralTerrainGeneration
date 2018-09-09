#version 120
#extension GL_EXT_gpu_shader4 : require

flat varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;

uniform float time;

void main(){
    gl_Position = ftransform();
    // water
    if (gl_Vertex.y < 7.f) gl_Position.y += ((sin(((mod(gl_Vertex.x+gl_Vertex.z,31*6.f))+time)/200.f)-0.5f));
    
    // pass through
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
