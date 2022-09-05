#pragma once
class Object
{
public:
	std::string name;
	std::vector<Mesh> meshes;

	Object() = default;

	Object(std::vector<Mesh> meshes)
	{
		name = "";
		this->meshes = meshes;
		n = meshes.size();

		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, biggestAxis, size_x, size_y, size_z;

		min_x = max_x = meshes[0].vertices[0].position.x;
		min_y = max_y = meshes[0].vertices[0].position.y;
		min_z = max_z = meshes[0].vertices[0].position.z;

		for (int i = 0; i < n; i++)
		{
			int m = meshes[i].vertices.size();
			for (int j = 0; j < m; j++)
			{
				Vertex vertex = meshes[i].vertices[j];
				if (vertex.position.x < min_x) min_x = vertex.position.x;
				if (vertex.position.x > max_x) max_x = vertex.position.x;
				if (vertex.position.y < min_y) min_y = vertex.position.y;
				if (vertex.position.y > max_y) max_y = vertex.position.y;
				if (vertex.position.z < min_z) min_z = vertex.position.z;
				if (vertex.position.z > max_z) max_z = vertex.position.z;
			}
		}

		// Initialize model matrix components
		translation = std::make_shared<glm::vec3>(0.0, 0.0, 0.0);
		scale = std::make_shared<glm::vec3>(1.0, 1.0, 1.0);
		rotation = glm::vec3(0.0f, 0.0f, 1.0f);

		// Normalize mesh into unitary parallelepiped
		size_x = max_x - min_x;
		size_y = max_y - min_y;
		size_z = max_z - min_z;

		biggestAxis = std::max(std::max(size_x, size_y), size_z);
		normalize = glm::vec3(1/biggestAxis, 1/biggestAxis, 1/biggestAxis);


		// Create bounding box
		glm::vec3 size = glm::vec3(size_x, size_y, size_z);
		boundingBox = BoundingBox(glm::vec3(min_x, min_y, min_z), glm::vec3(max_x, max_y, max_z),
								size, normalize, translation, scale);
	}

	void setName(std::string name) 
	{
		this->name = name;
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

	glm::vec3* getScaleFactor()
	{
		return &(*scale);
	}

	glm::vec3* getTranslationFactor()
	{
		return &(*translation);
	}

	glm::mat4 getModelTransformation()
	{
		return
			glm::translate(glm::mat4(1.0f), *translation) *
			glm::scale(glm::mat4(1.0f), *scale) * 
			glm::scale(glm::mat4(1.0f), normalize);
	}
	BoundingBox boundingBox;
private:
	int n;
	
	// Model Tranformation
	glm::vec3 normalize;
	std::shared_ptr<glm::vec3> scale, translation;
	glm::vec3 rotation;
	float angle = 45.0;
};

std::vector<Object> objects;

bool draw = false;
Object *selectedObject;