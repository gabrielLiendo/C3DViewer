#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

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
	sampler2D diffuseMap;
    sampler2D specularMap;
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
	float shininess;
};

uniform Light gLight;
uniform Material gMaterial;
uniform vec3 view;

void main()
{
	float specularStrength = 5;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(view - FragPos);
	vec3 lightDir = normalize(gLight.position - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 

	vec4 ambientColor = vec4(gLight.color, 1.0) *
						gLight.ambientIntensity * 
						vec4(gMaterial.ambientColor, 1.0);

	vec4 diffuseColor = vec4(gLight.color, 1.0f)*
						gLight.diffuseIntensity * 
						vec4(gMaterial.diffuseColor, 1.0) *
						max(dot(norm, lightDir), 0.0);
	
	vec4 specularColor = vec4(gLight.color, 1.0)*
						 specularStrength * 
						 pow(max(dot(viewDir, reflectDir), 0.0), gMaterial.shininess) *
						 vec4(gMaterial.specularColor, 1.0);


	vec3 textureColor = vec3(texture(gMaterial.diffuseMap, TexCoord)) + vec3(texture(gMaterial.specularMap, TexCoord));

	fragColor = (ambientColor + diffuseColor + specularColor) * textureColor;
} 