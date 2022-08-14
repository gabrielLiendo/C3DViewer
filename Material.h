#pragma once
class Material
{
public:
	std::string name;
	vec3 kd;

	Material(std::string name, vec3 diffuse)
	{
		this->name = name;
		this->kd = diffuse;
	}

	void setDiffuse(vec3 diffuse)
	{
		this->kd = diffuse;
	}

private:
	
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

std::vector<Material> materials;
Material* default_mtl;