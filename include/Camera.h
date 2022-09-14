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

	void changePosX(float delta)
	{
		position.x += delta;
	}

	void changePosY(float delta)
	{
		position.y += delta;
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