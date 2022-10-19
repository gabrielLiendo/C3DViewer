#pragma once
struct Scene
{
	// Models Information
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Material>> materials;
	Object* selectedObject = nullptr;

	// Camera
	Camera camera;

	// Light
	Light light;

	bool useLighting = false;

	// Background Color
	glm::vec3 bgColor;

	// Rendering State
	bool useDepthTest = true;
	bool useCullFace = true;
	bool useMultisample = true;

	Scene()
	{
		// Establish default background color
		bgColor = glm::vec3(0.2745f, 0.2745f, 0.2745f);

		// Create Default Material
		materials.push_back(std::make_shared<Material>());

		// Create Main Light Source
		light = Light();
	};

	void addMaterial(std::string name, glm::vec3 ka, glm::vec3 kd, glm::vec3 ks)
	{
		std::shared_ptr<Material> newMtl = std::make_shared<Material>(name, ka, kd, ks);
		materials.push_back(newMtl);
	}

	void setDepthTest()
	{
		if (useDepthTest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);
	}

	void setCullFace()
	{
		if (useCullFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);
	}

	void setMultisample()
	{
		if (useMultisample)
			glEnable(GL_MULTISAMPLE);
		else
			glDisable(GL_MULTISAMPLE);
	}

	void setSelectedObject(Object* selectedObject)
	{
		this->selectedObject = selectedObject;
	}

	void deleteAllObjects()
	{
		objects.clear();
		materials.resize(1);
		selectedObject = nullptr;
	}

	void deleteSelected()
	{
		if (selectedObject)
		{
			std::vector<Object>::size_type i = selectedObject - &objects[0];
			assert(i < objects.size());
			std::vector<Object>::iterator it = objects.begin() + i;
			objects.erase(it);
			selectedObject = nullptr;
		}
	}

	void saveInfo(std::ofstream& outfile)
	{	
		camera.saveInfo(outfile);
		
		// Write background color and rendering options booleans
        outfile << "bg " << bgColor.x << " " << bgColor.y << " " << bgColor.z << "\n"
				<< "ro " << useDepthTest << " " << useCullFace << " " << useMultisample << "\n";

        // Write each objects' properties
		int n = (int)objects.size();
        for (int i = 0; i < n; i++)
            objects[i].getInfo(outfile);
	}

	void loadInfo(std::ifstream& infile)
	{	
		std::string line, prefix;
    	std::stringstream ss;

		camera.loadInfo(infile);

		std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> bgColor.x >> bgColor.y >> bgColor.z;
        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> useDepthTest >>useCullFace >> useMultisample;

		deleteAllObjects(); 
	}
};