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
    vec3 ambientColor = gLight.color *
					    gLight.ambientIntensity * 
					    gMaterial.ambientColor;
    
	vec3 result = gMaterial.diffuseColor * ambientColor;
	
    fragColor = vec4(result, 1.0f);
}  