#pragma once
class SceneLayer
{
public:
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Material>> materials;

	Light light;
	bool useLighting = false;

	Object* selectedObject;

	SceneLayer()
	{
		// Create Default Material
		materials.push_back(std::make_shared<Material>());
		light = Light();
	};

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