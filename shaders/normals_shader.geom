#version 330 core

layout (points) in; 
layout (line_strip, max_vertices = 2) out;

uniform mat4 MVP;
uniform float normalScale;

in vec3 vs_normal[];

void main()
{
    vec4 v0 = gl_in[0].gl_Position;
    gl_Position = MVP * v0;
    EmitVertex();

    // We calculate v1 of our line segment
    vec4 v1 = v0 + vec4(vs_normal[0] * normalScale, 0);
    gl_Position = MVP * v1;
    EmitVertex();

    EndPrimitive();
}