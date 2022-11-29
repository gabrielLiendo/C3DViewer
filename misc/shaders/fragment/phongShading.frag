#version 330 core

struct Light
{
	vec3 color;
	vec3 position;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
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

struct ColorCombination
{
	bool useAmbMtlColor;
	bool useAmbTexColor;
	bool useDiffMtlColor;
	bool useDiffTexColor;
	bool useSpecMtlColor;
	bool useSpecTexColor;
};

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform Light gLight;
uniform Material gMaterial;
uniform ColorCombination gCombination;

out vec4 fragColor;

void main()
{
	vec3 N = normalize(Normal);
	vec3 L = normalize(gLight.position - FragPos);

	float lambertian = max(dot(N, L), 0.0);
	float specular = 0.0;
	if(lambertian > 0.0){
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-FragPos);
		float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, gMaterial.shininess);
	}
	
	// Ambient Color
	vec3 Iamb = gLight.color * gLight.ambientIntensity;
	if(gCombination.useAmbMtlColor)
	{
		Iamb = Iamb* gMaterial.ambientColor;
	}
	
	// Diffuse Color
	vec3 Idif = gLight.color * gLight.diffuseIntensity * lambertian;

	vec3 diffuseColor = vec3(1.0);
	if(gCombination.useDiffMtlColor)
	{
		diffuseColor = diffuseColor * gMaterial.diffuseColor;
	}
	if(gCombination.useDiffTexColor)
	{
		diffuseColor = diffuseColor * vec3(texture(gMaterial.diffuseMap, TexCoord));
	}

	// Specular Color
	vec3 Ispe = gLight.color * gLight.specularIntensity * specular;
	if(gCombination.useSpecMtlColor)
	{
		Ispe = Ispe* gMaterial.specularColor;
	}
	if(gCombination.useSpecTexColor)
	{
		Ispe = Ispe * vec3(texture(gMaterial.specularMap, TexCoord));
	}
				
	fragColor = vec4( (Iamb + Idif + Ispe) * diffuseColor, 1.0 );
} 