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
        this->objLoader = ObjLoader(scene);
    }

    // Open File Dialog and load .obj with its respective material (.mtl)
    void loadObj(int &selected)
    {
        char const* objFileName, * lFilterPatterns[1] = { "*.obj" };

        objFileName = tinyfd_openFileDialog("Open", "", 1, lFilterPatterns, NULL, 0);

        if (!objFileName)
            return;

        // Load the respective .mtl
        std::string mtlFileName = objFileName;
        mtlFileName = mtlFileName.substr(0, mtlFileName.size() - 4) + ".mtl";
        MtlLoader::load(mtlFileName.c_str(), scene);

        // Convert "i", the integer mesh ID, into an RGB color
        int i = (int) scene->objects.size();
        int r = (i & 0x000000FF) >> 0;
        int g = (i & 0x0000FF00) >> 8;
        int b = (i & 0x00FF0000) >> 16;
        glm::vec3 pickingColor = glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);

        // Load .obj
        Object newObject = Object(objLoader.loadMeshes(objFileName), pickingColor, objFileName);
        scene->objects.push_back(newObject);

        // Set obj name
        std::string objName = objFileName;
        objName = objName.substr(objName.find_last_of("/\\") + 1);
        objName = objName.substr(0, objName.size() - 4);
        scene->objects.back().name = objName;

        // Set loaded object as selected
        selected = (int)(scene->objects.size() - 1);
        scene->selectedObject = &scene->objects.back();
    }

    // Save the current scene in a .txt
    void saveScene()
    {
        char const* lTheSaveFileName;
        char const* lFilterPatterns[2] = { "*.txt", "*.text" };
        FILE* lIn;

        lTheSaveFileName = tinyfd_saveFileDialog("Save As", "Scene.txt", 2, lFilterPatterns, nullptr);

        if (!lTheSaveFileName)
            return;

        lIn = fopen(lTheSaveFileName, "w");

        if (!lIn)
        {
            tinyfd_messageBox("Error", "Can not open this file in write mode", "Ok", "Error", 1);
            return;
        }

        // Open file for writing
        std::ofstream outfile;
        outfile.open(lTheSaveFileName);

        // Write the Camera's position
        glm::vec3 camPos = *scene->camera.getPosition();
        outfile << "camera " << camPos.x << " " << camPos.y << " " << camPos.z << "\n";

        // Write background color
        outfile << "bg " << scene->bgColor.x << " " << scene->bgColor.y << " " << scene->bgColor.z << "\n";

        // Write the rendering options booleans
        outfile << "ro " << scene->useDepthTest << " " << scene->useCullFace << " " << scene->useMultisample << "\n";

        // Write each objects' properties
        for (int i = 0; i < scene->objects.size(); i++)
            scene->objects[i].getInfo(outfile);

        outfile.close();
    }

    void loadScene()
    {
        char const* sceneFileName, * lFilterPatterns[2] = { "*.txt", "*.text" };

        scene->deleteAllObjects();

        sceneFileName = tinyfd_openFileDialog("Open Scene", "", 2, lFilterPatterns, NULL, 0);

        if (!sceneFileName)
            return;

        glm::vec3 camPos;

        infile = std::ifstream(sceneFileName);

        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> camPos.x >> camPos.y >> camPos.z;
        scene->camera.setPosition(camPos);
        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> scene->bgColor.x >> scene->bgColor.y >> scene->bgColor.z;
        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> scene->useDepthTest >> scene->useCullFace >> scene->useMultisample;

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);
            
            ss >> prefix;
            if (prefix == "o")
                scene->objects.push_back(loadObject());
        }
    }

    Object loadObject()
    {
        std::vector<Mesh> meshes;
        std::string buffName, name, mtl, buffPath, objPath, mtlPath;

        glm::vec3 normalize, scale, translation, angles;
        glm::vec3 pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, diffuseColor;
        glm::vec3 vmin, vmax;
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

       
        mtlPath = objPath.substr(0, objPath.size() - 4) + ".mtl";
        MtlLoader::load((mtlPath).c_str(), scene);
        meshes = objLoader.loadMeshes((objPath).c_str());

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);

            ss >> prefix;
            if (prefix == "o")
            {
                size_t size = line.size();
                infile.putback('\n');
                for (int i = 0; i < size; i++)
                    infile.putback(line[size - i - 1]);

                break;
            }
            if (prefix == "mtl")
            {
                ss >> mtl;
                std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> diffuseColor.x >> diffuseColor.y >> diffuseColor.z;
                for (size_t i = scene->materials.size() - 1; i >= 0; i--)
                {
                    if (*(scene->materials[i])->getName() == mtl)
                    {
                        (*scene->materials[i]).setDiffuse(diffuseColor);
                        break;
                    }
                }
            }
        }

        return Object(meshes, objPath, name, normalize, scale, translation, angles, 
            showWireframe, showVertices, showNormals, pointSize, pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, vmin, vmax);
    }

private:
    Scene* scene = nullptr;

    ObjLoader objLoader;

    std::ifstream infile;
    std::string line, prefix;
    std::stringstream ss;
};