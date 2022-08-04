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

	Model readModel()
	{
		std::vector<Mesh> meshes;

		std::stringstream ss;
		std::string line, prefix;

		while (std::getline(infile, line))
		{
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

		return Model(meshes);
	}

	Mesh readMesh()
	{
		std::vector<Vertex> face, vertices;

		std::stringstream ss, sv;
		std::string line, prefix, vertex;
		
		vec3 v3;
		//vec2 v2;

		unsigned int n, count, index, posindex, normalindex /*, textcoordindex*/ ;

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
				while (ss >> vertex)
				{
					sv.clear();
					sv.str(vertex);
					count = 0;
					posindex = 0; normalindex = 0 /*, textcoordindex = 0 */;
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
						/*else if (count == 1)
							textcoordindex = index;*/
						else if (count == 2)
							normalindex = index;
					}
					face.push_back({ positions[posindex - 1], normals[normalindex - 1], {0.8,0.8,0.8} });
				}
				
				if (face.size() > 3)
					face = triangulate(face);
				
				for (int i = 0; i < face.size(); i++)
					vertices.push_back(face[i]);
			}
		}

		return Mesh(vertices);
	}

	std::vector<Vertex> triangulate(std::vector<Vertex> face)
	{
		std::vector<Vertex> vertices;

		vertices.push_back(face[0]);
		vertices.push_back(face[1]);
		vertices.push_back(face[3]);

		vertices.push_back(face[1]);
		vertices.push_back(face[2]);
		vertices.push_back(face[3]);

		return vertices;
	}
};