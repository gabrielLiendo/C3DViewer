#version 330 core

layout (triangles) in; 
layout (triangles, max_vertices = 3) out;

uniform mat4 MVP;

in vec3 vs_normal[];

void main()
{
    vec4 v0 = gl_in[0].gl_Position;
    gl_Position = MVP * v0;
    EmitVertex();

    vec4 v1 = gl_in[1].gl_Position;
    gl_Position = MVP * v1;
    EmitVertex();

    vec4 v2 = gl_in[2].gl_Position;
    gl_Position = MVP * v2;
    EmitVertex();

    EndPrimitive();
}