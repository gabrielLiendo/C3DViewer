#pragma once
namespace MtlLoader
{
	std::vector<std::shared_ptr<Material>> load(const char* file_name)
	{	
		std::vector<std::shared_ptr<Material>> loadedMaterials;

		std::ifstream infile = std::ifstream(file_name);

		if (!infile.is_open())
			return {};
	
		std::string line, prefix;
		std::stringstream ss;
	
		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "newmtl")
			{
				int illum;
				float ns = 0.0f, ni, d;
				glm::vec3 ka, kd, ks, ke;
				std::string name, mapKa = "", mapKd = "", mapKs = "";

				// Read one mtl at the time
				ss >> name;
				while (std::getline(infile, line))
				{
					ss.clear();
					ss.str(line);

					ss >> prefix;
					// New mtl found, put back this line in stream
					if (prefix == "newmtl")
					{
						size_t size = line.size();
						infile.putback('\n');
						for (int i = 0; i < size; i++)
							infile.putback(line[size - i - 1]);
						break;
					}

					if (prefix == "Ka")
						ss >> ka.x >> ka.y >> ka.z;
					else if (prefix == "Kd")
						ss >> kd.x >> kd.y >> kd.z;
					else if (prefix == "Ks")
						ss >> ks.x >> ks.y >> ks.z;
					else if (prefix == "Ke")
						ss >> ke.x >> ke.y >> ke.z;
					else if (prefix == "Ns")
						ss >> ns;
					else if (prefix == "Ni")
						ss >> ni;
					else if (prefix == "d")
						ss >> d;
					else if (prefix == "illum")
						ss >> illum;
					else if (prefix == "map_Ka")
						ss >> mapKa;
					else if (prefix == "map_Kd")
						ss >> mapKd;
					else if (prefix == "map_Ks")
						ss >> mapKs;
				}
				
				std::string mtlPath = file_name;
				std::shared_ptr<Material> newMtl = std::make_shared<Material>(name, ka, kd, ks, ns);

				// Load and set textures maps
				if(mapKa != "")
				{	
					std::string texturePath = mtlPath.substr(0, mtlPath.find_last_of("/\\") + 1) + mapKa; 
					std::string textureName = texturePath.substr(texturePath.find_last_of("/\\") + 1, texturePath.size()); 
					std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(texturePath, mapKa, textureName);
                	newMtl->setTextureMap(newTexture, AMBIENT_MAP);
				}

				if(mapKd != "")
				{	
					std::string texturePath = mtlPath.substr(0, mtlPath.find_last_of("/\\") + 1) + mapKd; 
					std::string textureName = texturePath.substr(texturePath.find_last_of("/\\") + 1, texturePath.size()); 
					std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(texturePath, mapKd, textureName);
                	newMtl->setTextureMap(newTexture, DIFFUSE_MAP);
				}

				if(mapKs != "")
				{	
					std::string texturePath = mtlPath.substr(0, mtlPath.find_last_of("/\\") + 1) + mapKs; 
					std::string textureName = texturePath.substr(texturePath.find_last_of("/\\") + 1, texturePath.size()); 
					std::shared_ptr<Texture> newTexture = std::make_shared<Texture>(texturePath, mapKs, textureName);
                	newMtl->setTextureMap(newTexture, SPECULAR_MAP);
				}

				loadedMaterials.push_back(newMtl);
			}
		}

		return loadedMaterials;
	}
}