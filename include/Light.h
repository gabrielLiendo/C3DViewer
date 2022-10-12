#pragma once
class Light
{
public:
	Light()
	{
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		position = glm::vec3(1.0f, 1.0f, 2.0f);
		ambientIntensity = 1.0f;
		diffuseIntensity = 0.5f;
	}

	Light(glm::vec3 color, float ambientIntensity)
	{
		this->color = color;
		this->ambientIntensity = ambientIntensity;
	}

	// Getters
	glm::vec3* getColor(){ return &color; }
	glm::vec3* getPosition() { return &position; }
	float* getAmbientIntensity(){ return &ambientIntensity; }
	float* getDiffuseIntensity() { return &diffuseIntensity; }

private:
	glm::vec3 color;
	glm::vec3 position;
	float ambientIntensity, diffuseIntensity;
};