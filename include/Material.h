#pragma once
class Material
{
public:
	std::string name, kdMapName;
	std::shared_ptr<Texture> texture;

	Material()
	{
		name = "Default";
		ambient = glm::vec3(0.0, 0.0, 0.0);
		diffuse = glm::vec3(0.7, 0.7, 0.7);
		specular = glm::vec3(0.5, 0.5, 0.5);
		shininess = 20.0f;
		kdMapName = "";
	}

	Material(std::string name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess, std::string kdMapName)
	{
		this->name = name;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
		this->kdMapName = kdMapName;
	}

	void setTexture(std::shared_ptr<Texture> texture)
	{
		this->texture = texture;
	}
	
	std::string *getName()
	{
		return &name;
	}

	glm::vec3 *getDiffuse(){ return &diffuse; }

	glm::vec3 *getAmbient() { return &ambient; }

	glm::vec3 *getSpecular() { return &specular; }

	void setDiffuse(glm::vec3 diffuse)
	{
		this->diffuse = diffuse;
	}

	

private:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float shininess;

	/*
	vec3 emissive;
	float specularExponent;
	float opticalDensity;
	float transparency;
	short illum;
	*/
};