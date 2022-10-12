#pragma once
class Light
{
public:
	Light()
	{
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		ambientIntensity = 1.0f;
	}

	Light(glm::vec3 color, float ambientIntensity)
	{
		this->color = color;
		this->ambientIntensity = ambientIntensity;
	}

	// Getters
	glm::vec3* getColor(){ return &color; }
	float* getAmbientIntensity(){ return &ambientIntensity; }

private:
	glm::vec3 color;
	float ambientIntensity;
};