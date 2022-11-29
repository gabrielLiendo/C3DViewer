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

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform Light gLight;
uniform Material gMaterial;

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
	
	vec3 Iamb = gLight.color *
				gLight.ambientIntensity * 
				gMaterial.ambientColor;

	vec3 Idif = gLight.color *
				gLight.diffuseIntensity *
				lambertian *
				gMaterial.diffuseColor;
	
	vec3 Ispe = gLight.color *
				gLight.specularIntensity * 
				specular *
				gMaterial.specularColor *
				vec3(texture(gMaterial.specularMap, TexCoord));

	vec3 diffuseColor = vec3(texture(gMaterial.diffuseMap, TexCoord));

	fragColor = vec4( (Iamb + Idif + Ispe) * diffuseColor, 1.0 );
} 