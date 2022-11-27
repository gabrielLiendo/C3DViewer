#pragma once
class Material
{
public:
	std::string name;
	std::shared_ptr<Texture> kaMap, kdMap, ksMap;

	Material()
	{
		name = "Default";
		ambient = glm::vec3(0.0, 0.0, 0.0);
		diffuse = glm::vec3(0.7, 0.7, 0.7);
		specular = glm::vec3(0.5, 0.5, 0.5);
		shininess = 20.0f;
	}

	Material(std::string name, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess)
	{
		this->name = name;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->shininess = shininess;
	}

	// Change texture map
	void setTextureMap(std::shared_ptr<Texture> texMap, TextureMap map)
	{	
		switch (map)
		{
		case AMBIENT_MAP:
			this->kaMap = texMap;
			break;
		case DIFFUSE_MAP:
			this->kdMap = texMap;
			break;
		case SPECULAR_MAP:
			this->ksMap = texMap;
			break;
		default:
			break;
		}	
	}

	std::string *getName()
	{
		return &name;
	}

	glm::vec3 *getDiffuse(){ return &diffuse; }

	glm::vec3 *getAmbient() { return &ambient; }

	glm::vec3 *getSpecular() { return &specular; }

	float *getShininess() { return &shininess; }

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