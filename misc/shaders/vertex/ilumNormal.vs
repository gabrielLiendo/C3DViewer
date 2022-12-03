#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform mat4 MVP;
uniform mat4 model;

out vec3 Normal;
out vec2 TexCoord;
out vec3 VertPosition;

void main()
{
	Normal = mat3(transpose(inverse(model))) * normal;  
	VertPosition = vec3(model * vec4(position, 1.0));
	TexCoord = texCoord;
	
	gl_Position = MVP * vec4(position, 1.0);
}