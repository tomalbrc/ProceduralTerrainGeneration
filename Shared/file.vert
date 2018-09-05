#version 120

varying vec4 vertexWorldPos;
uniform float time;

void main(){
    gl_Position = ftransform();
    // water
    if (gl_Vertex.y < 6.f) gl_Position.y += ((sin(((mod(gl_Vertex.x+gl_Vertex.z,128*4-1))+time)/150.f)-0.5f));
    
    // pass through
    vertexWorldPos = gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
