#version 330 core

//interpolated normal
in vec3 Normal;  
in vec3 Color;  

// constant color
uniform vec3 objectColor;

// output color
out vec4 color;

void main()
{
	color = vec4(objectColor * Color, 1.0);
} 
