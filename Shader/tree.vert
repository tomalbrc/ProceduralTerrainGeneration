#version 120
#extension GL_EXT_gpu_shader4 : require // glsl extension for flat

flat varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;

uniform float time;

void main(){
    gl_Position = ftransform();
    // water
    float multi = (gl_Vertex.y/4.f); // 4.f == max model height (todo: set as callback)
    float val = (multi*multi*time)/300.f;
    gl_Position.x += sin(val)/4.f;
    
    // pass through
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
}





