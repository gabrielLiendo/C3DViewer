#pragma once
class DirectionalLight
{
public:
	// The default light is a directional light
	DirectionalLight()
	{
		direction = glm::vec3(1.0f, -1.0f, -5.0f);
		diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		diffuseIntensity = 1.0f;
		specularIntensity = 1.0f;
	}
	
	DirectionalLight(glm::vec3 direction, glm::vec3 diffuseColor, glm::vec3 specularColor, float diffuseIntensity, float specularIntensity)
	{	
		this->direction = direction;
		this->diffuseColor = diffuseColor;
		this->specularColor = specularColor;
		this->diffuseIntensity = diffuseIntensity;
		this->specularIntensity = specularIntensity;
	}

	// Getters
	glm::vec3* getDirection() { return &direction; }
	glm::vec3* getDiffuseColor(){ return &diffuseColor; }
	glm::vec3* getSpecularColor(){ return &specularColor; }
	float* getDiffuseIntensity() { return &diffuseIntensity; }
	float* getSpecularIntensity() { return &specularIntensity; }
	
private:
	glm::vec3 direction;
	glm::vec3 diffuseColor, specularColor;
	float diffuseIntensity, specularIntensity;
};

class PointLight : public DirectionalLight
{
public:
	PointLight() : DirectionalLight()
	{
		constant = 1.0f;
		linear = 1.0f;
		quadratic = 1.0f;
	}

	PointLight(glm::vec3 direction, glm::vec3 diffuseColor, glm::vec3 specularColor, float diffuseIntensity, float specularIntensity,
				float constant, float linear, float quadratic): DirectionalLight(direction, diffuseColor, specularColor, diffuseIntensity, specularIntensity)
	{
		this->constant = constant;
		this->linear = linear;
		this->quadratic = quadratic; 
	}

	float* getConstantComponent() { return &constant; }
	float* getLinearComponent() { return &linear; }
	float* getQuadraticComponent() { return &quadratic; }

private:
	float constant, linear, quadratic;
};