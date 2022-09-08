#version 330 core

uniform vec3 normalsColor; 

out vec4 fragColor;

void main()
{
	fragColor = vec4(normalsColor, 1.0f);
} 