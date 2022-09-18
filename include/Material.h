#pragma once
class Material
{
public:
	std::string name;
	
	Material(std::string name, glm::vec3 diffuse)
	{
		this->name = name;
		this->diffuse = diffuse;
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
	glm::vec3 diffuse;
	/*
	vec3 ambient;
	vec3 specular;
	vec3 emissive;
	float specularExponent;
	float opticalDensity;
	float transparency;
	short illum;
	*/
};

std::vector<std::shared_ptr<Material>> materials;
Material default_mtl = Material("Default", glm::vec3(0.7, 0.7, 0.7));