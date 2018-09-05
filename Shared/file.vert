#version 120

uniform float time;

// Input vertex data, different for all executions of this shader.
uniform vec3 vertexPosition_modelspace;
uniform vec4 mWorldViewProj;
uniform vec4 mTransWorld;

varying float height;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
