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
	vec3 ambientColor;
	vec3 diffuseColor;
	vec3 specularColor;
};

uniform Light gLight;
uniform Material gMaterial;
uniform vec3 view;
uniform sampler2D gSampler;

void main()
{
	float specularStrength = 5;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(view - FragPos);
	vec3 lightDir = normalize(gLight.position - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 

	vec4 ambientColor = vec4(gLight.color, 1.0f) *
						gLight.ambientIntensity * 
						vec4(gMaterial.ambientColor, 1.0f);

	vec4 diffuseColor = vec4(gLight.color, 1.0f)*
						gLight.diffuseIntensity * 
						vec4(gMaterial.diffuseColor, 1.0f) *
						max(dot(norm, lightDir), 0.0);
	
	vec3 specularColor = pow(max(dot(viewDir, reflectDir), 0.0), 32) *
						 specularStrength * 
						 gMaterial.specularColor *
						 gLight.color;  

	fragColor = (ambientColor + diffuseColor + specularColor) * texture(gSampler, TexCoord);;
} 