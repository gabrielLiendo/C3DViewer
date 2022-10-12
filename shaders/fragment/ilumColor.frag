#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 fragColor;

struct Light
{
	vec3 color;
	vec3 position;
	float ambientIntensity;
	float diffuseIntensity;
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
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(gLight.position - FragPos);

	vec4 ambientColor = vec4(gLight.color, 1.0f) *
						gLight.ambientIntensity * 
						vec4(gMaterial.ambientColor, 1.0f);
	
	float diffuseFactor = max(dot(norm, lightDir), 0.0);

	vec4 diffuseColor = vec4(gLight.color, 1.0f)*
						gLight.diffuseIntensity * 
						vec4(gMaterial.diffuseColor, 1.0f) *
						diffuseFactor;

	fragColor = (ambientColor + diffuseColor) * diffuseColor;
} 