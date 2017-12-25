#version 330 core
precision highp float;

out vec4 gl_FragColor;

in vec4 v_Color;

void main()
{
	gl_FragColor = v_Color;
}