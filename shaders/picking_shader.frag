#version 330 core

uniform vec3 pickingColor;

out vec4 fragColor;

void main()
{
	fragColor = vec4(pickingColor, 1.0f);
} 