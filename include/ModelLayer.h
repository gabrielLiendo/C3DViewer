#pragma once
class ModelLayer
{
public:
	std::vector<Object> objects;
	std::vector<std::shared_ptr<Material>> materials;
	Object* selectedObject;

	ModelLayer() = default;

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