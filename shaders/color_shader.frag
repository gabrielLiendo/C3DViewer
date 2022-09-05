#version 330 core

in vec3 diffuse;  
out vec4 frag_color;

void main()
{
	frag_color = vec4(diffuse, 1.0f);
} 