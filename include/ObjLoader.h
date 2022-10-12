#pragma once
class ObjLoader
{
public:
	ObjLoader() = default;

	ObjLoader(SceneLayer* sceneLayer)
	{
		this->sceneLayer = sceneLayer;
	}

	std::vector<Mesh> loadMeshes(const char* file_name)
	{
		checkNormals = true;
		normalsIncluded = true;

		infile = std::ifstream (file_name);

		return readMeshes();
	}

private:
	SceneLayer* sceneLayer;

	std::ifstream infile;
	std::vector<glm::vec3> positions, normals, faceNormals;
	std::vector<int> positionIndex;

	std::string line, prefix;
	std::stringstream ss;

	bool checkNormals = true;
	bool normalsIncluded = true;

	std::vector<Mesh> readMeshes()
	{
		glm::vec3 v3;
		std::vector<Mesh> meshes;

		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "o" || prefix == "g")
				meshes.push_back(readMesh());
			else if (prefix == "v")
			{
				ss >> v3.x >> v3.y >> v3.z;
				positions.push_back(v3);
			}
			else if (prefix == "vn")
			{
				ss >> v3.x >> v3.y >> v3.z;
				normals.push_back(v3);
			}
		}

		infile.close();

		positions.clear();
		normals.clear();
		faceNormals.clear();
		positionIndex.clear();

		return meshes;
	}

	Mesh readMesh()
	{
		unsigned int n, count, index, posindex, normalindex, textcoordindex;

		std::vector<int> faceIndexes;
		std::vector<Vertex> vertices, face;

		std::stringstream sv;
		std::string name, vertex, mtl_name = "Default";
		
		glm::vec3 v3, aNormal; //glm::vec2 v2;

		ss >> name;
		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "o")
			{	
				int size = line.size();
				infile.putback('\n');
				for (int i = 0; i < size; i++)
					infile.putback(line[size - i - 1]);

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
			else if (prefix == "usemtl")
				ss >> mtl_name;
			else if (prefix == "f")
			{
				if (checkNormals)
				{
					normalsIncluded = normals.size() > 0;
					checkNormals = false;
				}

				if(normalsIncluded)
				{
					face.clear();
					while (ss >> vertex)
					{
						sv.clear();
						sv.str(vertex);
						count = 0; posindex = 0; normalindex = 0, textcoordindex = 0;
						while (sv)
						{
							while (sv.peek() == '/')
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
						}

						face.push_back({ positions[posindex - 1], normals[normalindex - 1] });
					}

					if (face.size() == 3)
					{
						vertices.push_back({ face[0].position, face[0].normal });
						vertices.push_back({ face[1].position, face[1].normal }); 
						vertices.push_back({ face[2].position, face[2].normal });
					}
					else
					{
						// Triangulate
						vertices.push_back({ face[0].position, face[0].normal });
						vertices.push_back({ face[1].position, face[1].normal });
						vertices.push_back({ face[3].position, face[3].normal });

						vertices.push_back({ face[1].position, face[1].normal });
						vertices.push_back({ face[2].position, face[2].normal });
						vertices.push_back({ face[3].position, face[3].normal });
					}
				}
				else
				{   // Obj doesnt posses normals
					faceIndexes.clear();
					while (ss >> vertex)
					{
						sv.clear();
						sv.str(vertex);
						count = 0, posindex = 0, textcoordindex = 0;
						while (sv)
						{
							while (sv.peek() == '/')
							{
								count++;
								sv.ignore(1, '/');
							}

							sv >> index;
							if (count == 0)
								posindex = index;
							else if (count == 1)
								textcoordindex = index;
						}

						faceIndexes.push_back(posindex);
						
					}

					if (faceIndexes.size() == 3)
					{
						int pos0 = faceIndexes[0]; int pos1 = faceIndexes[1]; int pos2 = faceIndexes[2];
						aNormal = normalize(cross(positions[pos1 - 1] - positions[pos0 - 1], positions[pos2 - 1] - positions[pos0 - 1]));
						faceNormals.push_back(aNormal);

						positionIndex.push_back(pos0);
						positionIndex.push_back(pos1);
						positionIndex.push_back(pos2);
					}
					else
					{
						// Triangulate
						int pos0 = faceIndexes[0]; int pos1 = faceIndexes[1]; int pos2 = faceIndexes[2]; int pos3 = faceIndexes[3];
						aNormal = normalize(cross(positions[pos1 - 1] - positions[pos0 - 1], positions[pos3 - 1] - positions[pos0 - 1]));
						faceNormals.push_back(aNormal);
						aNormal = normalize(cross(positions[pos2 - 1] - positions[pos1 - 1], positions[pos3 - 1] - positions[pos1 - 1]));
						faceNormals.push_back(aNormal);

						positionIndex.push_back(pos0);
						positionIndex.push_back(pos1);
						positionIndex.push_back(pos3);

						positionIndex.push_back(pos1);
						positionIndex.push_back(pos2);
						positionIndex.push_back(pos3);
					}
				}
			}
		}

		if (!normalsIncluded)
		{
			for (int i = 0; i < positionIndex.size(); i++)
			{
				glm::vec3 tNormal = glm::vec3(0,0,0);
				int n = 0;
				for (int j = 0; j < positionIndex.size(); j++)
				{
					if (positionIndex[i] == positionIndex[j] && i != j)
					{
						tNormal += faceNormals[i / 3];
						n++;
					}
				}
				tNormal /= n;
				std::cout << positionIndex[i] - 1 << " " << glm::normalize(tNormal).x << " " << glm::normalize(tNormal).y << " " << glm::normalize(tNormal).z << std::endl;
				vertices.push_back({ positions[positionIndex[i] - 1], glm::normalize(tNormal) });
			}
		}

		std::shared_ptr<Material> mtl = nullptr;
		for (int i = 0; i < (*sceneLayer).materials.size(); i++)
		{
			if (*((*sceneLayer).materials[i])->getName() == mtl_name)
				mtl = (*sceneLayer).materials[i];
		}
		
		if (!mtl)
		{
			std::cout << "El material " << mtl_name << " no fue encontrado." << std::endl;
			mtl = (* sceneLayer).materials[0];
		}

		Mesh newMesh = Mesh(name, vertices, mtl);

		faceIndexes.clear();
		vertices.clear(); 
		face.clear();

		return newMesh;
	}
};