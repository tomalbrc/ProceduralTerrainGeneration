#version 120

varying vec4 vertexWorldPos;
uniform sampler2D textureUnit0;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float val = vertexWorldPos.y/1024.f;
    gl_FragColor = vec4(val,val,val,1.f);
    //gl_FragColor = vec4(depth/300.f);
    
    vec4 color = texture2D(textureUnit0, gl_TexCoord[0].xy);
    gl_FragColor += color - vec4(depth/(1024.f*16.f));
    if (vertexWorldPos.y > 80.f*4.f) {
        gl_FragColor += vec4(vertexWorldPos.y/100.f);
    } else if (vertexWorldPos.y < 6.f*4.f) {
        gl_FragColor.y += vertexWorldPos.y/6.f/4.f;
        gl_FragColor.x -= vertexWorldPos.y/6.f*0.03f;
        gl_FragColor.z -= vertexWorldPos.y/6.f*0.03f;
    }
}

/*
 #version 120
 
 varying vec4 vertexWorldPos;
 uniform sampler2D textureUnit0;
 
 void main() {
 float depth = gl_FragCoord.z / gl_FragCoord.w;
 float val = vertexWorldPos.y/1024.f;
 gl_FragColor = vec4(val,val,val,1.f);
 
 vec4 color = vec4(80.f/255.f,40.f/255.f,10.f/255.f,1.f);
 if (vertexWorldPos.y > 80.f*4.f) {
 color += vec4(vertexWorldPos.y/512.f);
 } else if (vertexWorldPos.y < 6.f*4.f) {
 color = -vec4(vertexWorldPos.y/512.f) + vec4(70.f/255.f,240.f/255.f,20.f/255.f,1.f) + vec4(0,vertexWorldPos.y/(6.f*4.f),0,0) + vec4(vertexWorldPos.y/512.f);
 }
 gl_FragColor += color-vec4(depth/(1024.f*12.f));
 }
 
 */
