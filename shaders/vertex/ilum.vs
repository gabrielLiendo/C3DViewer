#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 MVP;
uniform mat4 model;

out vec3 Normal;
out vec3 FragPos;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0));
	Normal = mat3(transpose(inverse(model))) * normal;  
}