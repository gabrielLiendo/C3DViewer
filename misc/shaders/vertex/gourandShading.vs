#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

struct Light
{
	vec3 position;
	vec3 diffuseColor;
	vec3 specularColor;
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

uniform vec3 ambientColor;
uniform float ambientIntensity;
uniform mat4 model;
uniform mat4 MVP;
uniform Light gLight;
uniform Material gMaterial;
uniform ColorCombination gCombination;

out vec3 lightingColor;
out vec3 diffuseColor;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    
    vec3 VertexPos = vec3(model * vec4(position, 1.0));
    vec3 N = normalize(mat3(transpose(inverse(model))) * normal);
    vec3 L = normalize(gLight.position - VertexPos);
  	
    float lambertian = max(dot(N, L), 0.0);
    float specular = 0.0;
	if(lambertian > 0.0){
		vec3 R = reflect(-L, N);
		vec3 V = normalize(-VertexPos);
		float specAngle = max(dot(R, V), 0.0);
		specular = pow(specAngle, gMaterial.shininess);
	}

    // Ambient Color
	vec3 Iamb = ambientColor * ambientIntensity;
	if(gCombination.useAmbMtlColor)
	{
		Iamb = Iamb* gMaterial.ambientColor;
	}
	
	// Diffuse Color
	vec3 Idif = gLight.diffuseColor * gLight.diffuseIntensity * lambertian;

	vec3 objectColor = vec3(1.0);
	if(gCombination.useDiffMtlColor)
	{
		objectColor = objectColor * gMaterial.diffuseColor;
	}
	if(gCombination.useDiffTexColor)
	{
		objectColor = objectColor * vec3(texture(gMaterial.diffuseMap, texCoord));
	}

	// Specular Color
	vec3 Ispe = gLight.specularColor * gLight.specularIntensity * specular;
	if(gCombination.useSpecMtlColor)
	{
		Ispe = Ispe* gMaterial.specularColor;
	}
	if(gCombination.useSpecTexColor)
	{
		Ispe = Ispe * vec3(texture(gMaterial.specularMap, texCoord));
	}

	lightingColor = Iamb + Idif + Ispe;
	diffuseColor = objectColor;
}