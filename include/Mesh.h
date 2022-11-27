#pragma once
class Mesh
{
public:
	std::string name, mtlName;
	std::vector<Vertex> vertices;
	std::shared_ptr<Material> mtl;

	Mesh(std::string name, std::string mtlName, std::vector<Vertex> vertices)
	{
		this->name = name;
		this->vertices = vertices;
		this->mtlName = mtlName;
		showTriangles = true;

		initMesh();
	}

	void setMaterial(std::shared_ptr<Material> mtl)
	{
		this->mtl = mtl;
	}

	bool* getShow()
	{
		return &showTriangles;
	}

	std::string getName()
	{
		return name;
	}

	void draw()
	{
		bind();
		glDrawArrays(GL_TRIANGLES, 0, (int)vertices.size());
	}
	
	void drawVertex()
	{
		bind();
		glDrawArrays(GL_POINTS, 0, (int)vertices.size());
	}

	void getInfo(std::ofstream& outfile)
	{
		glm::vec3 diffuse = *mtl->getDiffuse();
		outfile << "om " << name << "\n"
			    << "mtl " << *mtl->getName() << "\n"
			    << "d " << diffuse.x << " " << diffuse.y  << " " << diffuse.z << "\n";
	}

private:
	unsigned int VAO, VBO;
	bool showTriangles;
	
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

		// Texture attribute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, textCoord));
		glEnableVertexAttribArray(2);  
		
	}

	void bind()
	{	
		if(mtl->kdMap != nullptr)
			mtl->kdMap->bind(GL_TEXTURE0);

		if(mtl->ksMap != nullptr)
			mtl->ksMap->bind(GL_TEXTURE1);
			
		glBindVertexArray(VAO);
	}
};