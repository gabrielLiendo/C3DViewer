#pragma once
class Scene
{
public:
	// Models Information
	std::vector<std::shared_ptr<Object>> objects;
	std::vector<std::shared_ptr<Material>> materials;
	std::shared_ptr<Object> selectedObject = nullptr;

	// Camera
	Camera camera;

	// Lights
	Light newLight = Light(true, false);
	std::vector<Light> lights;

	int numberOfLights = 1;

	int lightingModel = 2;
	int shadingModel = 2;

	// Colors
	glm::vec3 bgColor;
	glm::vec3 ambientColor = glm::vec3(1.0, 1.0, 1.0);
	float ambientIntensity = 1.0f;

	// Rendering State
	bool useDepthTest = true;
	bool useCullFace = true;
	bool useMultisample = true;

	// Establish default values for scene
	Scene()
	{
		bgColor = glm::vec3(0.2745f, 0.2745f, 0.2745f);
		materials.push_back(std::make_shared<Material>());
		// QUITAR ESTA LINEA
		lights.push_back(Light(true, false));
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

	void setSelectedObject(std::shared_ptr<Object> selectedObject)
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
			for(auto it = objects.begin(); it != objects.end(); ++it)
			{
				if ((*it) == selectedObject)
				{
					objects.erase(it);
					selectedObject = nullptr;
					return;
				}
			}
		}
	}

	bool addNewLight(LightType light)
	{
		if(numberOfLights >=3)
			return false;

		numberOfLights++;

		
		if(light==POINT)
		{
			newLight.isDirectional = false;
			newLight.isPoint = true;	
		}

		lights.push_back(newLight);
		newLight = Light(true, false);
		return true;
	}

	void removeLight(LightType light, int index)
	{
		lights.erase( lights.begin() + index );
		numberOfLights--;
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
            objects[i]->getInfo(outfile);
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