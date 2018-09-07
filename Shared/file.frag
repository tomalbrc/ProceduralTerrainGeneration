#version 120
#extension GL_EXT_gpu_shader4 : require

varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;
uniform sampler2D textureUnit0;


void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float val = vertexWorldPos.y/1024.f;
    gl_FragColor = vec4(val,val,val,1.f);
    
    vec4 color = texture2D(textureUnit0, gl_TexCoord[0].xy);
    gl_FragColor += color + vec4(depth/(1024.f*12.f));
    if (vertexWorldPos.y > 300.f) {
        gl_FragColor += vec4(vertexWorldPos.y/100.f);
    }
    
    vec3 sun = vec3(0.5f, 15.0f, 1.5f);
    vec3 light = max(vec3(0,0,0), dot(normalize(fragNormal), normalize(sun)));
    
    light = light / vec3(2.f) + vec3(.5f);
    
    gl_FragColor *= vec4(light.xyz,1.f);
}

