#version 120
#extension GL_EXT_gpu_shader4 : require

 varying vec4 vertexWorldPos;
 varying vec3 fragNormal;
uniform sampler2D textureUnit0;

void main() {
    vec3 sun = vec3(10.5f, 10.0f, 10.5f)*10.f;
    vec3 light = max(vec3(0), dot(normalize(fragNormal), normalize(sun))) / vec3(4.f) + 0.9f;
    vec4 color = texture2D(textureUnit0, gl_TexCoord[0].xy);
    gl_FragColor = color * vec4(light.xyz,1.f);
}
