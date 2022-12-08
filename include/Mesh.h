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
	
	~Mesh()
	{
		vertices.clear();
		mtl = nullptr;
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

	void draw(Shader shader)
	{	
		if(mtl->kaMap)
		{
			shader.setInt("gMaterial.ambientMap", 0);
			shader.setBool("gMaterial.ambMapLoaded", true);
			mtl->kdMap->activate(0);
		}
		else 
			shader.setBool("gMaterial.ambMapLoaded", false);

		if(mtl->kdMap)
		{
			shader.setInt("gMaterial.diffuseMap", 1);
			shader.setBool("gMaterial.diffMapLoaded", true);
			mtl->kdMap->activate(1);
		}
		else	
			shader.setBool("gMaterial.diffMapLoaded", false);

		if(mtl->ksMap)
		{
			shader.setInt("gMaterial.specularMap", 2);
			shader.setBool("gMaterial.specMapLoaded", true);
			mtl->ksMap->activate(2);
		}
		else
			shader.setBool("gMaterial.specMapLoaded", false);
			
		bind();
		glDrawArrays(GL_TRIANGLES, 0, (int)vertices.size());
		
		if(mtl->kaMap)
			mtl->kaMap->unbind();
		if(mtl->kdMap)
			mtl->kdMap->unbind();
		if(mtl->ksMap)
			mtl->ksMap->unbind();
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

	void resetData()
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
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
		glBindVertexArray(VAO);
	}
};