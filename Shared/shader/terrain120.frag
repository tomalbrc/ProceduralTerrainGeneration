#version 120
#extension GL_EXT_gpu_shader4 : require

varying vec4 vertexWorldPos;
flat varying vec3 fragNormal;
uniform sampler2D textureUnit0;
uniform sampler2D textureUnit1;
uniform sampler2D textureUnit2;
uniform sampler2D textureUnit3;
uniform vec3 lightSource;

void main() {
    vec3 addVal = vec3(.7f);
    
    vec4 color0 = texture2D(textureUnit0, gl_TexCoord[0].xy/0.01f); // sand
    vec4 color1 = texture2D(textureUnit1, gl_TexCoord[0].xy/0.01f); // grass
    vec4 color2 = texture2D(textureUnit2, gl_TexCoord[0].xy/0.01f); // rock
    vec4 color3 = texture2D(textureUnit3, gl_TexCoord[0].xy/0.01f); // snow

    // eper
    float perc = vertexWorldPos.y/256;
    
    
    if (perc > 0.8f) {
        gl_FragColor = mix(color2,color3, smoothstep(0.8f, 1.0f, perc))/2.f;
    } else if (perc > 0.2f) {
        gl_FragColor = mix(color1,color2, smoothstep(0.5f, .8f, perc))/2.f;
    } else {
        gl_FragColor = mix(color0,color1, smoothstep(0.0f, 0.1, perc))/2.f;
    }

    float depth = gl_FragCoord.z / gl_FragCoord.w;
    gl_FragColor += gl_FragColor + vec4(depth/(1024.f*10.f))+vec4(0.05f);
    
}

