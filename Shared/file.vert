#version 330

uniform float time;


// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

void main(){
    //gl_Position =  vec4(vertexPosition_modelspace,1);
    
    
    // add time to the noise parameters so it's animated
    float displacement = sin(time);
    
    vec3 newPosition = vertexPosition_modelspace * displacement;
    gl_Position = mWorldViewProj * mTransWorld * vec4(newPosition, 1.0);
}
