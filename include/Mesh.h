#pragma once
class Mesh
{
public:
	std::string name;
	std::vector<Vertex> vertices;
	Material* mtl;

	Mesh(std::string name, std::vector<Vertex> vertices, Material* mtl)
	{
		this->name = name;
		this->vertices = vertices;
		this->mtl = mtl;

		initMesh();
	}

	void draw()
	{
		bind();
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	}
	
	void drawVertex()
	{
		bind();
		glDrawArrays(GL_POINTS, 0, vertices.size());
	}

private:
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
	}

	void bind()
	{
		glBindVertexArray(VAO);
	}
};