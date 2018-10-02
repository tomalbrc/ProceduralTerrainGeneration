#version 130
//#extension GL_EXT_gpu_shader4 : require

flat in vec4 vertexWorldPos;
flat in vec3 fragNormal;
uniform vec3 lightSource;
uniform sampler2D textureUnit0;

void main() {
    float val = vertexWorldPos.y/1024.f;
    gl_FragColor = vec4(val,val,val,1.f);
    
    vec3 addVal = vec3(.7f);
    
    //vec4 color = texture2D(textureUnit0, gl_TexCoord[0].xy);
    vec4 color = vec4(0);
    if (vertexWorldPos.y > 300.f) {
        color += vec4(vertexWorldPos.y/300.f);
        addVal /= 1.5f;
    } else if (vertexWorldPos.y > 100.f) { // mountain brown
        color = vec4(80.f/255.f,40.f/255.f,10.f/255.f,1.f);
    } else if (vertexWorldPos.y > 94.f) { // red sandstone
        color = vec4(130.f/255.f,16.f/255.f,20.f/255.f,1.f);
    } else if (vertexWorldPos.y > 10.f) { // grass
        color = vec4(70.f/255.f,240.f/255.f,20.f/255.f,1.f);
    } else if (vertexWorldPos.y > 6.f) { // sand
        color = vec4(230.f/255.f,240.f/255.f,70.f/255.f,1.f);
    } else { // water
        color = vec4(0.f,100.f/255.f,1.f,1.f);
    }
    
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    gl_FragColor += color + vec4(depth/(1024.f*12.f));
    
    vec3 sun = lightSource;
    vec3 light = max(vec3(0), dot(normalize(fragNormal), normalize(sun))) / vec3(4.f) + addVal;
    gl_FragColor *= vec4(light.xyz,1.f);
}

