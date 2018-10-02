#version 130
//#extension GL_EXT_gpu_shader4 : require // glsl extension for flat

flat out vec4 vertexWorldPos;
flat out vec3 fragNormal;

uniform float time;

void main(){
    gl_Position = ftransform();

    // pass through
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
}





