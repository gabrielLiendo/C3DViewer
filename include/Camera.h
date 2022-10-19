#pragma once
class Camera
{
public:
	glm::vec3 position, front, up;

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

	// Update View matrix given the lastest values of the position, front and up vectors
	void updateView(){ view = glm::lookAt(position, position + front, up); }

	// Change front of camera given xoffset and yoffset
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
	
	// Save camera info into ofstream
	void saveInfo(std::ofstream& outfile)
	{
		 outfile << "position " << position.x << " " << position.y << " " << position.z << "\n"
		 		 << "front "    << front.x << " " << front.y << " " << front.z << "\n"
				 << "up "       << up.x << " " << up.y << " " << up.z << "\n"
				 << "fValue "   << yaw << " " << pitch << " " << speed << "\n";
	}

	// Load camera info from ifstream
	void loadInfo(std::ifstream& infile)
	{
		std::string line, prefix;
    	std::stringstream ss;

		std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> position.x >> position.y >> position.z;
		std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> front.x >> front.y >> front.z;
		std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> up.x >> up.y >> up.z;
		std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> yaw >> pitch >> speed;

		updateView();
	}

private:
    glm::mat4 view;
	float yaw, pitch, speed;
};