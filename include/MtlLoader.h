#pragma once
class MtlLoader
{
public:
	MtlLoader() = default;

	MtlLoader(ModelLayer* modelLayer)
	{
		this->modelLayer = modelLayer;
	}

	bool load(const char* file_name)
	{
		infile = std::ifstream(file_name);

		if (!infile.is_open()) {
			std::cout << file_name << " no fue conseguido" << std::endl;
			return false;
		}
			
		readMaterials();
	}
private:
	ModelLayer* modelLayer;

	std::string line, prefix;
	std::stringstream ss;
	std::ifstream infile;
	//std::vector<vec2> text_coords;

	void readMaterials()
	{
		std::string name;

		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "newmtl") 
				(*modelLayer).materials.push_back(readMaterial());
				
		}
	}

	std::shared_ptr <Material> readMaterial()
	{	
		int illum;
		float ns, ni, d;
		glm::vec3 ka, kd, ks, ke;
		std::string name;

		ss >> name;

		while (std::getline(infile, line))
		{
			ss.clear();
			ss.str(line);

			ss >> prefix;
			if (prefix == "newmtl")
			{
				int size = line.size();
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
		}
		
		return std::make_shared<Material>(name, ka, kd, ks);
	}
};