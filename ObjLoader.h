#pragma once

struct vec2
{
	float x, y;
};

struct vec3
{
	float x, y, z;
};

struct Vertex
{
	vec3 Position;
	vec3 Normal;
	vec3 Color;
};

class Mesh
{
public:
	Mesh(){}
	Mesh(std::vector<Vertex> vertices)
	{
		this->vertices = vertices;
		initMesh();
	}

	void draw()
	{
		bind();
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glBindVertexArray(0);
	}
private:
	unsigned int VAO, VBO;
	std::vector<Vertex> vertices;

	void initMesh()
	{
		// Gen and bind container's VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Gen and bind VBO, We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// Normal attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(1);

		// Color attribute
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	void bind()
	{
		glBindVertexArray(VAO);
	}
};

Mesh currentMesh;
bool draw = false;

class ObjLoader
{
public:
	ObjLoader() {}

	bool load(const char* file_name)
	{
		std::ifstream infile(file_name);
		
		if (!infile.is_open())
			return false;

		std::vector<vec3> positions;
		std::vector<vec3> normals;
		std::vector<vec2> text_coords;
		
		std::vector<Vertex> face;
		std::vector<Vertex> vertices;


		std::stringstream ss, sv;
		std::string line, prefix, vertex;
		
		vec2 v2;
		vec3 v3;

		unsigned int n, count, index, posindex, textcoordindex, normalindex;

		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "v")
			{
				ss >> v3.x >> v3.y >> v3.z;
				positions.push_back(v3);
			}
			else if (prefix == "vn")
			{
				ss >> v3.x >> v3.y >> v3.z;
				normals.push_back(v3);
			}
			else if (prefix == "vt")
			{
				ss >> v2.x >> v2.y;
				text_coords.push_back(v2);
			}
			else if (prefix == "f")
			{
				face.clear();
				n = 0;
				while (ss >> vertex)
				{
					sv.clear();
					sv.str(vertex);
					count = 0;
					posindex = 0; textcoordindex = 0; normalindex = 0;
					while(sv)
					{
						while(sv.peek() == '/')
						{
							count++;
							sv.ignore(1, '/');
						}
						sv >> index;
						if (count == 0)
							posindex = index;
						else if (count == 1)
							textcoordindex = index;
						else if (count == 2)
							normalindex = index;
						else
						{
							std::cout << "Invalid vertex" << std::endl;
						}
					}
					vertices.push_back({ positions[posindex - 1], normals[normalindex - 1], {1.0,1.0,0.0} });
				}
		
				if (n > 3)
					std::cout << "triangulizar !" << std::endl;
			}
		}

		std::cout << positions.size() << std::endl;
		std::cout << normals.size() << std::endl;
		std::cout << text_coords.size() << std::endl;
		std::cout << vertices.size() << std::endl;



		/*
		for (int i = 0; i < vertices.size(); i++)
		{
			std::cout << vertices[i].Position.x << " " << vertices[i].Position.y << " " << vertices[i].Position.z << " " << vertices[i].Normal.x << " " << vertices[i].Normal.y << " " << vertices[i].Normal.z << " " << vertices[i].Color.x << " " << vertices[i].Color.y << " " << vertices[i].Color.z<< std::endl;
		}
		*/
		draw = true;
		currentMesh = Mesh(vertices);

		return true;
	}
};