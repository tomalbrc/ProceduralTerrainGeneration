#version 120

varying vec4 vertexWorldPos;
uniform sampler2D textureUnit0;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    //gl_FragData[0] = vec4(depth/300.f);
    gl_FragColor = vec4(vertexWorldPos.x/300.f,vertexWorldPos.y/300.f,vertexWorldPos.z/300.f,1.f);//vec4(depth/300.f);
    //gl_FragColor = vec4(depth/300.f);
    
    vec4 color = texture2D(textureUnit0, gl_TexCoord[0].xy);
    //gl_FragColor = color;
    
}
