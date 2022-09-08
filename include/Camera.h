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

	void changePosZ(float delta)
	{
		position.z += delta;
	}

private:
	glm::vec3 position;
};

Camera camera;
bool selectedCamera;