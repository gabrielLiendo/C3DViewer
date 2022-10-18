#pragma once
class Camera
{
public:
	Camera() {
		position = glm::vec3(0.0, 0.0, 3.0);
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		up = glm::vec3(0.0f, 1.0f, 0.0f);

		speed = 2.5f;
		yaw = -90.f;
		pitch = 0.0f;

		view = glm::lookAt(position, position + front, up);
	}

	glm::mat4 getView(){ return view; }

	void setPosition(glm::vec3 position)
	{
		this->position = position;
	}

	glm::vec3* getPosition()
	{
		return &position;
	}

	void updateView(){ view = glm::lookAt(position, position + front, up); }

	void changeDirection(float xoffset, float yoffset)
	{	
		glm::vec3 direction;

		yaw   += 0.1f * xoffset;
    	pitch += 0.1f * yoffset;

		if(pitch > 89.0f)
        	pitch = 89.0f;
    	if(pitch < -89.0f)
        	pitch = -89.0f;
		
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
		updateView();
	}

	// Update position and view moving to the front or back
	void moveFrontBack(float delta)
	{
		position += speed * delta * front;
		updateView();
	}

	// Update position and view moving to the left or right
	void moveLeftRight(float delta)
	{
		position += speed * delta * glm::normalize(glm::cross(front, up));
		updateView();
	}
	
private:
	glm::vec3 position, front, up;
    glm::mat4 view;
	float yaw, pitch, speed;
};