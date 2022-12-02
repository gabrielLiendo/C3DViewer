#version 330 core

struct DirectionalLight
{
	vec3 direction;
	vec3 diffuseColor;
	vec3 specularColor;
	float diffuseIntensity;
	float specularIntensity;
};

struct PointLight
{
	vec3 position;
	vec3 diffuseColor;
	vec3 specularColor;
	float diffuseIntensity;
	float specularIntensity;
	float constant;
	float linear;
	float quadratic;
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
	int lightingModel;
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

uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform int nDirLights;
uniform int nPointLights;
uniform DirectionalLight gDirLights[3];
uniform PointLight gPointLights[3];

uniform Material gMaterial;
uniform ColorCombination gCombination;

out vec4 fragColor;

vec3 getAmbient()
{
	vec3 Iamb = ambientColor * ambientIntensity;
	if(gCombination.useAmbMtlColor)
	{
		Iamb = Iamb * gMaterial.ambientColor;
	}
	return Iamb;
}

vec3 getDiffuse(float lambertian, vec3 diffuseColor, float diffuseIntensity)
{
	vec3 Idif = diffuseColor * diffuseIntensity * lambertian;
	if(gCombination.useDiffMtlColor)
	{
		Idif = Idif  * gMaterial.diffuseColor;
	}
	if(gCombination.useDiffTexColor)
	{
		Idif = Idif  * vec3(texture(gMaterial.diffuseMap, TexCoord));
	}
	return Idif;
}

vec3 getSpecular(vec3 N, vec3 L, float lambertian, vec3 specularColor, float specularIntensity)
{
	float specular = 0.0;
	if(lambertian > 0.0)
	{
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-FragPos);
		float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, gMaterial.shininess);
	}

	vec3 Ispe = specularColor * specularIntensity * specular;
	if(gCombination.useSpecMtlColor)
	{
		Ispe = Ispe* gMaterial.specularColor;
	}
	if(gCombination.useSpecTexColor)
	{
		Ispe = Ispe * vec3(texture(gMaterial.specularMap, TexCoord));
	}

	return Ispe;
}

vec3 CalcDirectionalLight(DirectionalLight light)
{
	// Light contribution
	vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);
	ambient = getAmbient();

	if (gCombination.lightingModel != 0)
	{   
		vec3 N = normalize(Normal);
		vec3 L = normalize(-light.direction);
		float lambertian = max(dot(N, L), 0.0);

		diffuse = getDiffuse(lambertian, light.diffuseColor, light.diffuseIntensity);

		if(gCombination.lightingModel == 1)
		{	
			specular = getSpecular(N, L, lambertian, light.specularColor, light.specularIntensity);
		}
	}

    return (ambient + diffuse + specular);
} 

vec3 CalcPointLight(PointLight light)
{
    // Attenuation
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist +  light.quadratic * (dist * dist));    

	// Light contribution
	vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);
	ambient = getAmbient() * attenuation;

	if (gCombination.lightingModel != 0)
	{   
		vec3 N = normalize(Normal);
		vec3 L = normalize(light.position - FragPos);
		float lambertian = max(dot(N, L), 0.0);

		diffuse = getDiffuse(lambertian, light.diffuseColor, light.diffuseIntensity) * attenuation;

		if(gCombination.lightingModel == 1)
		{	
			specular = getSpecular(N, L, lambertian, light.specularColor, light.specularIntensity) * attenuation;
		}
	}

    return (ambient + diffuse + specular);
} 

void main()
{	
	// Object Color
	vec3 objectColor = vec3(1.0);
	if(gCombination.useDiffMtlColor)
	{
		objectColor = objectColor * gMaterial.diffuseColor;
	}
	if(gCombination.useDiffTexColor)
	{
		objectColor = objectColor * vec3(texture(gMaterial.diffuseMap, TexCoord));
	}

	vec3 lightReflection;

	for(int i=0; i < nDirLights; i++)
		lightReflection += CalcDirectionalLight(gDirLights[i]);

	for(int i=0; i < nPointLights; i++)
		lightReflection += CalcPointLight(gPointLights[i]);
	
	fragColor = vec4( lightReflection * objectColor, 1.0 );
} 