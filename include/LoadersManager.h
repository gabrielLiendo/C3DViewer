#pragma once
class LoadersManager
{
public:
    LoadersManager()
    {
        this->scene = nullptr;
    }

    LoadersManager(Scene* scene)
    {
        this->scene = scene;
    }

    // Assign the loaded materials to each mesh of an object
    void assignMaterials(std::vector<Mesh> &meshes, std::string objPath)
    {   
        // Load the respectives materials
        std::string mtlPath = objPath;
        mtlPath = mtlPath.substr(0, mtlPath.size() - 4) + ".mtl";
        std::vector<std::shared_ptr<Material>> loadedMaterials = MtlLoader::load(mtlPath.c_str());

        std::string mtlName = mtlPath;
        mtlName = mtlName.substr(mtlName.find_last_of("/\\") + 1);

        if(loadedMaterials.empty())
            std::cout << "The file " << mtlName << " was not found" << std::endl;

        int n = (int)meshes.size();
        int m = (int)loadedMaterials.size();

        // Assign material to each mesh
        for(int i=0; i < n; i++)
        {
            std::shared_ptr<Material> mtl = nullptr;
            std::string meshMtl = meshes[i].mtlName;

            for (int j=0; j < m; j++)
            {
                if (loadedMaterials[j]->name == meshMtl)
                    mtl = loadedMaterials[j];
            }
            if (!mtl)
                mtl = scene->materials[0];

            meshes[i].setMaterial(mtl);
        }

        // Add the loaded material to the list of materials
        for(int i=0; i < m; i++)
            scene->materials.push_back(loadedMaterials[i]);
    }

    // Open File Dialog and load .obj with its respective material (.mtl)
    void loadObj(int &selected)
    {
        char const* lFilterPatterns[1] = { "*.obj" };
        char const* objFileName = tinyfd_openFileDialog("Open", "", 1, lFilterPatterns, NULL, 0);

        if (!objFileName)
            return;

        // Get obj name
        std::string objName = objFileName;
        objName = objName.substr(objName.find_last_of("/\\") + 1);
        objName = objName.substr(0, objName.size() - 4);

        // Convert "i", the integer mesh ID, into an RGB color
        int i = (int) scene->objects.size();
        int r = (i & 0x000000FF) >> 0;
        int g = (i & 0x0000FF00) >> 8;
        int b = (i & 0x00FF0000) >> 16;
        glm::vec3 pickingColor = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

        // Load meshes and assign materials
        std::vector<Mesh> meshes = objLoader.loadMeshes(objFileName);
        assignMaterials(meshes, objFileName);
        scene->objects.push_back(std::make_shared<Object>(meshes, pickingColor, objFileName));
        scene->objects.back()->name = objName;
        
        // Set loaded object as selected
        selected = (int)(scene->objects.size() - 1);
        scene->selectedObject = scene->objects.back();
    }

    // Save the current scene in a .txt
    void saveScene()
    {
        char const* lFilterPatterns[2] = { "*.txt", "*.text" };
        char const* lTheSaveFileName  = tinyfd_saveFileDialog("Save As", "Scene.txt", 2, lFilterPatterns, nullptr);
  
        if (!lTheSaveFileName)
            return;

        FILE* lIn = fopen(lTheSaveFileName, "w");

        if (!lIn)
        {
            tinyfd_messageBox("Error", "Can not open this file in write mode", "Ok", "Error", 1);
            return;
        }

        // Open file for writing
        std::ofstream outfile;
        outfile.open(lTheSaveFileName);
        
        // Write scene info into file
        scene->saveInfo(outfile);

        // Close file
        outfile.close();
    }

    void loadScene()
    {
        char const* lFilterPatterns[2] = { "*.txt", "*.text" };
        char const* sceneFileName = tinyfd_openFileDialog("Open Scene", "", 2, lFilterPatterns, NULL, 0);

        if (!sceneFileName)
            return;

        infile = std::ifstream(sceneFileName);

        scene->loadInfo(infile);

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);
            
            ss >> prefix;
            if (prefix == "o")
                scene->objects.push_back(loadObject());
        }
    }

    std::shared_ptr<Object> loadObject()
    {
        std::vector<Mesh> meshes;
        std::string buffName, name, mtl, buffPath, objPath, mtlPath;

        glm::vec3 normalize, scale, translation, angles;
        glm::vec3 pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, diffuseColor;
        glm::vec3 vmin, vmax;
        glm::mat4 rotationMat;
        bool showWireframe, showVertices, showNormals;
        int pointSize;

        while (ss >> buffName)
            name += buffName + " ";
        name.pop_back();

        std::getline(infile, line);  ss.clear(); ss.str(line);  ss >> prefix;
        while (ss >> buffPath) 
            objPath += buffPath + " ";
        objPath.pop_back();

        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> normalize.x >> normalize.y >> normalize.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> scale.x >> scale.y >> scale.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> translation.x >> translation.y >> translation.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> rotationMat[0][0] >> rotationMat[0][1] >> rotationMat[0][2]
                                                                            >> rotationMat[1][0] >> rotationMat[1][1] >> rotationMat[1][2]
                                                                            >> rotationMat[2][0] >> rotationMat[2][1] >> rotationMat[2][2]
                                                                            >> rotationMat[3][0] >> rotationMat[3][1] >> rotationMat[3][2];
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> angles.x >> angles.y >> angles.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> showWireframe >> showVertices >> showNormals;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> pointSize;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> pickingColor.x >> pickingColor.y >> pickingColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> wireframeColor.x >> wireframeColor.y >> wireframeColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vertexColor.x >> vertexColor.y >> vertexColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> normalsColor.x >> normalsColor.y >> normalsColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> boxColor.x >> boxColor.y >> boxColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmin.x >> vmin.y >> vmin.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmax.x >> vmax.y >> vmax.z;

        meshes = objLoader.loadMeshes((objPath).c_str());
        assignMaterials(meshes, objPath);

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);

            ss >> prefix;
            if (prefix == "o")
            {
                int size = (int)line.size();
                infile.putback('\n');
                for (int i = 0; i < size; i++)
                    infile.putback(line[size - i - 1]);

                break;
            }
            if (prefix == "mtl")
            {
                ss >> mtl;
                std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> diffuseColor.x >> diffuseColor.y >> diffuseColor.z;
                for (int i = (int)scene->materials.size() - 1; i >= 0; i--)
                {
                    if (*scene->materials[i]->getName() == mtl)
                    {
                        scene->materials[i]->setDiffuse(diffuseColor);
                        break;
                    }
                }
            }
        }

        return std::make_shared<Object>(meshes, objPath, name, normalize, scale, translation, rotationMat, angles, 
            showWireframe, showVertices, showNormals, pointSize, pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, vmin, vmax);
    }

private:
    Scene* scene = nullptr;

    ObjLoader objLoader;

    std::ifstream infile;
    std::string line, prefix;
    std::stringstream ss;
};