#pragma once
class LoadersManager
{
public:
    // Open File Dialog and load .obj with its respective material (.mtl)
    void loadObj(int &selected)
    {
        FILE* lIn;
        char const* objFileName, * lFilterPatterns[1] = { "*.obj" };

        objFileName = tinyfd_openFileDialog("Open", "", 1, lFilterPatterns, NULL, 0);

        if (!objFileName)
            return;

        // Load the respective .mtl
        std::string mtlFileName = objFileName;
        mtlFileName = mtlFileName.substr(0, mtlFileName.size() - 4) + ".mtl";
        mtlLoader.load(mtlFileName.c_str());

        // Convert "i", the integer mesh ID, into an RGB color
        int i = objects.size() + 1;
        int r = (i & 0x000000FF) >> 0;
        int g = (i & 0x0000FF00) >> 8;
        int b = (i & 0x00FF0000) >> 16;
        glm::vec3 pickingColor = glm::vec3(r, g, b);

        // Load .obj
        Object newObject = Object(objLoader.loadMeshes(objFileName), pickingColor, objFileName);
        objects.push_back(newObject);

        // Set obj name
        std::string objName = objFileName;
        objName = objName.substr(objName.find_last_of("/\\") + 1);
        objName = objName.substr(0, objName.size() - 4);
        objects.back().setName(objName);

        // Set loaded object as selected
        selected = objects.size();
        selectedObject = &objects.back();
    }

    // Save the current scene in a .txt
    static void saveScene()
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
        glm::vec3 camPos = *camera.getPosition();
        outfile << "camera " << camPos.x << " " << camPos.y << " " << camPos.z << "\n";

        // Write background color
        outfile << "bg " << bgColor.x << " " << bgColor.y << " " << bgColor.z << "\n";

        // Write each objects' properties
        for (int i = 0; i < objects.size(); i++)
            objects[i].getInfo(outfile);

        outfile.close();
    }

    void loadScene()
    {
        FILE* lIn;
        char const* sceneFileName, * lFilterPatterns[2] = { "*.txt", "*.text" };

        deleteAllObjects();

        sceneFileName = tinyfd_openFileDialog("Open Scene", "", 2, lFilterPatterns, NULL, 0);

        if (!sceneFileName)
            return;

        glm::vec3 camPos;
        infile = std::ifstream(sceneFileName);

        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> camPos.x >> camPos.y >> camPos.z;
        camera.setPos(camPos);
        std::getline(infile, line); ss.clear(); ss.str(line);  ss >> prefix >> bgColor.x >> bgColor.y >> bgColor.z;

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);
            
            ss >> prefix;
            if (prefix == "o")
                objects.push_back(loadObject());    
        }
    }

    Object loadObject()
    {
        std::vector<Mesh> meshes;
        std::string buffName, name, mtl, buffPath, objPath, mtlPath;

        glm::vec3 normalize, scale, translation, angles;
        glm::vec3 pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, diffuseColor;
        glm::vec3 vmin, vmax;
        bool useDepthTest, useCullFace, useMultisample, showWireframe, showVertices, showNormals;
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
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> useDepthTest >> useCullFace >> useMultisample >> showWireframe >> showVertices >> showNormals;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> pointSize;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> pickingColor.x >> pickingColor.y >> pickingColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> wireframeColor.x >> wireframeColor.y >> wireframeColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vertexColor.x >> vertexColor.y >> vertexColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> normalsColor.x >> normalsColor.y >> normalsColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> boxColor.x >> boxColor.y >> boxColor.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmin.x >> vmin.y >> vmin.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmax.x >> vmax.y >> vmax.z;

       
        mtlPath = objPath.substr(0, objPath.size() - 4) + ".mtl";
        mtlLoader.load((mtlPath).c_str());
        meshes = objLoader.loadMeshes((objPath).c_str());

        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);

            ss >> prefix;
            if (prefix == "o")
            {
                int size = line.size();
                infile.putback('\n');
                for (int i = 0; i < size; i++)
                    infile.putback(line[size - i - 1]);

                break;
            }
            if (prefix == "mtl")
            {
                ss >> mtl;
                std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> diffuseColor.x >> diffuseColor.y >> diffuseColor.z;
                for (int i = materials.size() - 1; i >= 0; i--)
                {
                    if ((*materials[i]).name == mtl)
                    {
                        (*materials[i]).setDiffuse(diffuseColor);
                        break;
                    }
                }
            }
        }

        return Object(meshes, objPath, name, normalize, scale, translation, angles, useDepthTest, useCullFace, useMultisample,
            showWireframe, showVertices, showNormals, pointSize, pickingColor, wireframeColor, vertexColor, normalsColor, boxColor, vmin, vmax);
    }

private:
    ObjLoader objLoader;
    MtlLoader mtlLoader;

    std::ifstream infile;
    std::string line, prefix;
    std::stringstream ss;
};