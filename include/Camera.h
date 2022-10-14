#pragma once
class Camera
{
public:
	Camera() {
		position = glm::vec3(0.0, 0.0, 2.0);
	}

	glm::vec3* getPosition()
	{
		return &position;
	}

	void setPos(glm::vec3 position)
	{
		this->position = position;
	}

	void changePosX(double xoffset)
	{
		position.x += xoffset;
	}

	void changePosY(double yoffset)
	{
		position.y += yoffset;
	}

	void changePosZ(double delta)
	{
		position.z += delta;
	}
	
private:
	glm::vec3 position;
};