#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 MVP;
uniform mat4 model;

out vec2 vTexCoord;
out vec3 vWorldPos;

void main()
{
	vWorldPos = vec3(model * vec4(position, 1.0));
	vTexCoord = texCoord;
	
	gl_Position = MVP * vec4(position, 1.0);
}