#version 330 core
in vec4 fColor;
out vec4 color;

in vec2 texcoord;

uniform sampler2D u_textureMap;

void main()
{
    color =texture2D(u_textureMap,texcoord)*fColor;   
}