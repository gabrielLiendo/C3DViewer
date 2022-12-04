#pragma once
class Light
{
public:
	bool isDirectional;
	bool isPoint;

	// The default light is a directional light
	Light(bool isDirectional, bool isPoint)
	{
		this->isDirectional = isDirectional;
		this->isPoint = isPoint;

		direction = glm::vec3(1.0f, -1.0f, -5.0f);
		diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
		specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		diffuseIntensity = 1.0f;
		specularIntensity = 1.0f;
		constant = 1.0f;
		linear = 0.1f;
		quadratic = 0.01f;
		
	}


	// Getters
	glm::vec3* getDirection() { return &direction; }
	glm::vec3* getDiffuseColor(){ return &diffuseColor; }
	glm::vec3* getSpecularColor(){ return &specularColor; }
	float* getDiffuseIntensity() { return &diffuseIntensity; }
	float* getSpecularIntensity() { return &specularIntensity; }

	float* getConstantComponent() { return &constant; }
	float* getLinearComponent() { return &linear; }
	float* getQuadraticComponent() { return &quadratic; }
	
private:
	glm::vec3 direction;
	glm::vec3 diffuseColor, specularColor;
	float diffuseIntensity, specularIntensity;
	float constant, linear, quadratic;
};