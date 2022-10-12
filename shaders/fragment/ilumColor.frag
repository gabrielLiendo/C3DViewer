#version 330 core

out vec4 fragColor;

struct Light
{
	vec3 color;
	float ambientIntensity;
};

struct Material
{
	vec3 ambientColor;
	vec3 diffuseColor;
};

uniform Light gLight;
uniform Material gMaterial;

void main()
{
	fragColor = vec4(gMaterial.diffuseColor, 1.0f) * 
				vec4(gMaterial.ambientColor, 1.0f) * 
				vec4(gLight.color, 1.0f) *
			    gLight.ambientIntensity;
} 