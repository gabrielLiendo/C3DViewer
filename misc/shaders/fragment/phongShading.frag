#version 330 core

struct Light
{
	vec3 direction;
	vec3 diffuseColor;
	vec3 specularColor;
	float diffuseIntensity;
	float specularIntensity;
	float constant;
	float linear;
	float quadratic;
	bool isPoint;
	bool isDirectional;
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
in vec2 TexCoord;
in vec3 VertPosition;

uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float ambientIntensity;

uniform int nLights;
uniform Light gLights[3];

uniform Material gMaterial;
uniform ColorCombination gCombination;

out vec4 FragColor;

vec3 getAmbient()
{
	vec3 Iamb = ambientColor * ambientIntensity;
	if(gCombination.useAmbMtlColor)
	{
		Iamb = Iamb * gMaterial.ambientColor;
	}
	return Iamb;
}

vec3 getDiffuse(float diff, int i)
{
	vec3 Idif = gLights[i].diffuseColor * diff * gLights[i].diffuseIntensity;
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

vec3 getSpecular(float spec, int i)
{
	vec3 Ispe = gLights[i].specularColor * spec * gLights[i].specularIntensity;
	if(gCombination.useSpecMtlColor)
	{
		Ispe = Ispe * gMaterial.specularColor;
	}
	if(gCombination.useSpecTexColor)
	{
		Ispe = Ispe * vec3(texture(gMaterial.specularMap, TexCoord));
	}

	return Ispe;
}

void main()
{	
	vec3 N = normalize(Normal);
	vec3 viewDir = normalize(viewPos - VertPosition);

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

	// Compute scattered and reflected light
	vec3 ambient = vec3(0.0), diffuse = vec3(0.0), specular = vec3(0.0);

	ambient = getAmbient();

	for(int i=0; i < nLights; i++)
	{
		if (gCombination.lightingModel != 0)
		{  	
			vec3 L; 
			float attenuation = 1.0;
			if(gLights[i].isDirectional)
			{	
				L = normalize(-gLights[i].direction);
			}
			else if(gLights[i].isPoint)
			{
				vec3 lightPos = gLights[i].direction;
				float dist = length(lightPos - VertPosition);
				L = normalize(lightPos- VertPosition);
				attenuation = 1.0 / (gLights[i].constant + gLights[i].linear * dist +  gLights[i].quadratic * (dist * dist));    
			}
				
			float diff = max(dot(N, L), 0.0);
			
			diffuse += getDiffuse(diff, i) * attenuation;

			if(gCombination.lightingModel == 2)
			{	
				if(diff > 0.0)
				{
					vec3 R = reflect(-L, N);
					float specAngle = max(dot(R, viewDir), 0.0);
					float spec = pow(specAngle, gMaterial.shininess);
					specular += getSpecular(spec, i) * attenuation;
				}
			}
		}
	}

	vec3 rgb = min(objectColor * (ambient + diffuse + specular), vec3(1.0));
	FragColor = vec4(rgb , 1.0);
} 