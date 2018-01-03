#version 330 core

precision highp float;

out vec4 gl_FragColor;

in vec2 v_Coord;
in vec4 v_particleColor;

uniform sampler2D u_textureMap;

void main() {

    gl_FragColor = (texture2D(u_textureMap, v_Coord)*v_particleColor);
}