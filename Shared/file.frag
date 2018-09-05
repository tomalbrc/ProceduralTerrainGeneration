#version 120

varying float height;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    gl_FragData[0] = vec4(depth/300.f);
}
