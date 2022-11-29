#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

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

uniform mat4 model;
uniform mat4 MVP;
uniform Light gLight;
uniform Material gMaterial;

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
				vec3(texture(gMaterial.specularMap, texCoord));

	lightingColor = Iamb + Idif + Ispe;
	diffuseColor = vec3(texture(gMaterial.diffuseMap, texCoord));
}