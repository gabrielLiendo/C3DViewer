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
	vec3 specularColor;
};

uniform Light gLight;
uniform Material gMaterial;
uniform vec3 view;

void main()
{
	float specularStrength = 0.5;
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(gLight.position - FragPos);
	vec3 viewDir = normalize(view - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 

	vec4 ambientColor = vec4(gLight.color, 1.0f) *
						gLight.ambientIntensity * 
						vec4(gMaterial.ambientColor, 1.0f);
	
	float diffuseFactor = max(dot(norm, lightDir), 0.0);

	vec4 diffuseColor = vec4(gLight.color, 1.0f)*
						gLight.diffuseIntensity * 
						vec4(gMaterial.diffuseColor, 1.0f) *
						diffuseFactor;
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specularColor = specularStrength * spec * 
						 gMaterial.specularColor *
						 gLight.color;  

	fragColor = (ambientColor + diffuseColor + specularColor) * diffuseColor;
} 