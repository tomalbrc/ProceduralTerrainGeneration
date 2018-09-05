#version 120

varying vec4 vertexWorldPos;
uniform vec3 mLightPos;

uniform float time;

void main(){
    gl_Position = ftransform();
    if (gl_Position.y < 10 && mod(gl_Position.y,2.f) == 0.f) gl_Position.y += sin(time)*4.f;
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Position * vec4();
    vertexWorldPos = gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
