#pragma once
class ObjLoader
{
public:
	ObjLoader() = default;

	bool load(const char* file_name)
	{
		infile = std::ifstream (file_name);

		if (!infile.is_open())
			return false;

		readModel();
	}

private:
	std::ifstream infile;
	std::vector<vec3> positions;
	std::vector<vec3> normals;
	//std::vector<vec2> text_coords;

	void readModel()
	{
		std::vector<Mesh> meshes;

		std::stringstream ss;
		std::string line, prefix;

		while (std::getline(infile, line))
		{
			std::cout << line << std::endl;
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "o")
			{
				meshes.push_back(readMesh());
			}
		}

		currentModel = Model(meshes);
		draw = true;

		infile.close();
		positions.clear();
		normals.clear();
	}

	Mesh readMesh()
	{
		std::cout << "LEER MESH" << std::endl;
	
		std::vector<Vertex> face;
		std::vector<Vertex> vertices;

		std::stringstream ss, sv;
		std::string line, prefix, vertex;
		
		//vec2 v2;
		vec3 v3;

		unsigned int n, count, index, posindex, textcoordindex, normalindex;

		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "o")
			{
				infile.putback('\n');
				infile.putback('o');
				break;
			}
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
			/*else if (prefix == "vt")
			{
				ss >> v2.x >> v2.y;
				text_coords.push_back(v2);
			}*/
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
		std::cout << vertices.size() << std::endl;

		return Mesh(vertices);
	}
};