#pragma once
class Material
{
public:
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

	glm::vec3 *getDiffuse()
	{
		return &diffuse;
	}

	void setDiffuse(glm::vec3 diffuse)
	{
		this->diffuse = diffuse;
	}

private:
	std::string name;
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