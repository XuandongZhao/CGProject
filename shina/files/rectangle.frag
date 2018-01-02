#version 330 core

precision highp float;

out vec4 gl_FragColor;

in vec2 v_Coord;

uniform sampler2D u_textureMap;

void main() {
    vec3 fragColor = texture2D(u_textureMap, v_Coord).rgb;

    gl_FragColor = vec4(fragColor, 1.0);
}