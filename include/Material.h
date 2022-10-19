#pragma once
class Material
{
public:
	std::string name;

	Material()
	{
		name = "Default";
		ambient = glm::vec3(1.0, 1.0, 1.0);
		diffuse = glm::vec3(0.7, 0.7, 0.7);
		specular = glm::vec3(0.5, 0.5, 0.5);
	}

	Material(std::string name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	{
		this->name = name;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
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

	/*
	vec3 emissive;
	float specularExponent;
	float opticalDensity;
	float transparency;
	short illum;
	*/
};