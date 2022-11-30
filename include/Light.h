#pragma once
class Light
{
public:
	Light()
	{
		position = glm::vec3(1.0f, 1.0f, 2.0f);
		diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		diffuseIntensity = 1.0f;
		specularIntensity = 1.0f;
	}

	

	Light(glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		this->diffuseColor = diffuseColor;
		this->specularColor = specularColor;
	}

	// Getters
	glm::vec3* getPosition() { return &position; }
	glm::vec3* getDiffuseColor(){ return &diffuseColor; }
	glm::vec3* getSpecularColor(){ return &specularColor; }
	float* getDiffuseIntensity() { return &diffuseIntensity; }
	float* getSpecularIntensity() { return &specularIntensity; }
	
private:
	glm::vec3 position;
	glm::vec3 diffuseColor, specularColor;
	float diffuseIntensity, specularIntensity;
};