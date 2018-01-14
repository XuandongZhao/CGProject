#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in VS_OUT {
    vec4 color;
} gs_in[];

out vec4 fColor;
out vec2 texcoord;
uniform mat4 u_projection;
uniform mat4 u_view;

void build_house(vec4 position)
{    
    fColor = gs_in[0].color; // gs_in[0] since there's only one input vertex
	
    gl_Position = u_projection * u_view *(position + vec4(-0.11f, -0.11f, 0.0f, 0.0f));    // 1:bottom-left   
	texcoord=vec2(0,0);
    EmitVertex();   
    gl_Position = u_projection * u_view *(position + vec4( 0.11f, -0.11f, 0.0f, 0.0f));    // 2:bottom-right
	texcoord=vec2(1,0);
    EmitVertex();
    gl_Position = u_projection * u_view *(position + vec4(-0.11f,  0.11f, 0.0f, 0.0f));    // 3:top-left
	texcoord=vec2(0,1);
    EmitVertex();
    gl_Position = u_projection * u_view *(position + vec4( 0.11f,  0.11f, 0.0f, 0.0f));    // 4:top-right
	texcoord=vec2(1,1);
    EmitVertex();
    EndPrimitive();
}

void main() {    
    build_house(gl_in[0].gl_Position);
}