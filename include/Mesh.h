#pragma once
class Mesh
{
public:
	std::string name;
	std::vector<Vertex> vertices;

	Mesh(std::string name, std::vector<Vertex> vertices, Material* mtl)
	{
		this->name = name;
		this->vertices = vertices;
		this->mtl = mtl;

		// Set bounding box's corners values
		float min_x, max_x, min_y, max_y, min_z, max_z, biggestAxis, size_x, size_y, size_z;

		min_x = max_x = vertices[0].position.x;
		min_y = max_y = vertices[0].position.y;
		min_z = max_z = vertices[0].position.z;

		for (int i = 0; i < vertices.size(); i++) {
			if (vertices[i].position.x < min_x) min_x = vertices[i].position.x;
			if (vertices[i].position.x > max_x) max_x = vertices[i].position.x;
			if (vertices[i].position.y < min_y) min_y = vertices[i].position.y;
			if (vertices[i].position.y > max_y) max_y = vertices[i].position.y;
			if (vertices[i].position.z < min_z) min_z = vertices[i].position.z;
			if (vertices[i].position.z > max_z) max_z = vertices[i].position.z;
		}

		// Normalize mesh into unitary parallelepiped
		size_x = max_x - min_x;
		size_y = max_y - min_y;
		size_z = max_z - min_z;
		biggestAxis = std::max(std::max(size_x, size_y), size_z);

		glm::vec3 size = glm::vec3(size_x/biggestAxis, size_y / biggestAxis, size_z / biggestAxis);
		

		boundingBox = BoundingBox(glm::vec3(min_x /biggestAxis, min_y / biggestAxis, min_z / biggestAxis), 
					glm::vec3(max_x / biggestAxis, max_y / biggestAxis, max_z / biggestAxis), size);

		initMesh();

		translation = glm::vec3(0.0f, 0.0f, 0.0f);
		rotation = glm::vec3(0.0f, 1.0f, 0.0f);
		scale = glm::vec3(1.0f/biggestAxis, 1.0f/biggestAxis, 1.0f/biggestAxis);
	}

	void draw()
	{
		bind();
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}

	void drawBoundingBox()
	{
		boundingBox.draw();
	}

	void setTranslation(float x, float y, float z)
	{
		translation.x = x;
		translation.y = y;
		translation.z = z;
	}

	glm::mat4 getModelTransformation()
	{
		return
			glm::translate(glm::mat4(1.0f), translation) *
			glm::rotate(glm::mat4(1.0f), angle, rotation) *
			glm::scale(glm::mat4(1.0f), scale);
	}
	BoundingBox boundingBox;
private:
	
	Material* mtl;

	// Model Tranformation
	glm::vec3 translation, rotation, scale;
	float angle;

	unsigned int VAO, VBO;
	
	void initMesh()
	{
		// Gen and bind container's VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Gen and bind VBO, We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		// Color attribute
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
		glEnableVertexAttribArray(2);
	}

	void bind()
	{
		glBindVertexArray(VAO);
	}
};

Mesh* selectedMesh;