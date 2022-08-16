#pragma once
class Model
{
public:
	std::vector<Mesh> meshes;

	Model() = default;

	Model(std::vector<Mesh> meshes)
	{
		this->meshes = meshes;
		n = meshes.size();
	}

	void draw()
	{
		for (int i = 0; i < n; i++)
			meshes[i].draw();
	}
private:
	int n;
};

std::vector<Model> models;


bool draw = false;
Model currentModel;