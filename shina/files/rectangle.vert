#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 a_Coord;

out vec2 v_Coord;

uniform mat4 u_projection;
uniform mat4 u_view;

uniform mat4 u_modelMatrix;

void main() {

    v_Coord = a_Coord;
    gl_Position = u_projection * u_view * u_modelMatrix * vec4(position, 1.0);;
}
