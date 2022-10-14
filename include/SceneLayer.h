#pragma once
class Scene
{
public:
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Material>> materials;

	Light light;
	bool useLighting = false;

	Object* selectedObject;

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
		selectedObject = nullptr;
		objects.clear();
		materials.clear();
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
};