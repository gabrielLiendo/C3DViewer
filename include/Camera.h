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

	void changePosX(float xoffset)
	{
		position.x += xoffset;
	}

	void changePosY(float yoffset)
	{
		position.y += yoffset;
	}

	void changePosZ(float delta)
	{
		position.z += delta;
	}
	glm::vec3 position;
private:
	
};

Camera camera;
bool selectedCamera;