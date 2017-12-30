#version 330 core

layout (location = 0) in vec3 position;

out vec4 v_Color;

uniform vec4 v_color;

uniform mat4 u_modelMatrix;

uniform mat4 u_projection;

uniform mat4 u_view;




void main()
{
	v_Color=v_color;
	gl_Position = u_projection * u_view * u_modelMatrix * vec4(position, 1.0);;
}
