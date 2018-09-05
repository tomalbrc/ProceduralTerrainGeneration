#version 120

attribute mat4 ViewMatrix;
varying vec4 vertexWorldPos;

void main(){
    gl_Position = ftransform();
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Position;
    
    vertexWorldPos = gl_ModelViewMatrix * gl_Vertex;
    vertexWorldPos = ViewMatrix * vertexWorldPos;
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
