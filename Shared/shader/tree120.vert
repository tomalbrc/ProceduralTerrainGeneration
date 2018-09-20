#version 120 
#extension GL_EXT_gpu_shader4 : require // glsl extension for flat

flat varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;

uniform float time;

void main(){
    gl_Position = ftransform();
    
    // wiggle // disabled for now
//    float multi = (gl_Vertex.y/5.f); // 4.f == max model height (todo: set as callback)
//    float val = (multi*time*2)/300.f;
//    gl_Position.x += sin(val/10.f)*multi;
//    gl_Position.y += cos(val/8.f)*multi;

    // pass through
    gl_TexCoord[0] = gl_MultiTexCoord0;
    vertexWorldPos = gl_Vertex;
    fragNormal = vec3(gl_Normal.xyz);
}





