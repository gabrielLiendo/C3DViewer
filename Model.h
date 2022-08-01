#pragma once
class Model
{
public:
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
	std::vector<Mesh> meshes;
};

bool draw = false;
Model currentModel;