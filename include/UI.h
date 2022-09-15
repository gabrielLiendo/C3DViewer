#pragma once
class UI
{
public:
    UI() = default;

    UI(GLFWwindow* window, glm::vec3* bgColor)
    {
        this->window = window;
        this->bgColor = bgColor;
    }

    static void DrawVec3Control(const std::string label, glm::vec3& values, float v_speed, float v_min, float v_max)
    {
        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoPadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text(label.c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::Button("X");
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::Button("Y");
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(3);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::Button("Z");
        ImGui::PopStyleColor(3);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndTable();
    }

    void objectsWindow()
    {
        if (ImGui::Begin("Objects"))
        {
            if (ImGui::Selectable("Camera", selected == 0))
            {
                selected = 0;
                selectedCamera = true;
                selectedObject = nullptr;
            }

            int m = objects.size();
            for (int i = 1; i < m + 1; i++)
            {
                if (ImGui::Selectable(objects[i - 1].getName().c_str(), selected == i))
                {
                    selected = i;
                    selectedObject = &objects[i - 1];
                }
            }

            ImGui::End();
        }
            
        if (ImGui::Begin("Rendering"))
        {

            if (selectedObject)
            {
                ImGui::Text("Enable Z-buffer");
                ImGui::SameLine();
                ImGui::Checkbox("##z-buffer", selectedObject->getZBufferBool());

                ImGui::Text("Enable Back-face Culling");
                ImGui::SameLine();
                ImGui::Checkbox("##backCulling", selectedObject->getCullFaceBool());

                ImGui::Text("Enable Antialiasing");
                ImGui::SameLine();
                ImGui::Checkbox("##antialiasing", selectedObject->getMultisampleBool());

                if (ImGui::TreeNode("Vertices"))
                {
                    ImGui::Text("Show Vertices");
                    ImGui::SameLine();
                    ImGui::Checkbox("##Vertices", selectedObject->getVerticesBool());
                    ImGui::Text("Vertex size");
                    ImGui::SameLine();
                    ImGui::DragInt("##vertexSize", selectedObject->getPointSize(), 1);
                    ImGui::Text("Color");
                    ImGui::SameLine();
                    ImGui::ColorEdit3("##VerticesColor", glm::value_ptr(*selectedObject->getVerticesColor()), ImGuiColorEditFlags_NoLabel);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Normals"))
                {
                    ImGui::Text("Show Normals");
                    ImGui::SameLine();
                    ImGui::Checkbox("##VNormals", selectedObject->getNormalsBool());
                    ImGui::Text("Color");
                    ImGui::SameLine();
                    ImGui::ColorEdit3("##NormalsColor", glm::value_ptr(*selectedObject->getNormalsColor()), ImGuiColorEditFlags_NoLabel);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Wireframe"))
                {
                    ImGui::Text("Show Wireframe");
                    ImGui::SameLine();
                    ImGui::Checkbox("##Wireframe", selectedObject->getWireframeBool());
                    ImGui::Text("Color");
                    ImGui::SameLine();
                    ImGui::ColorEdit3("##WireColor", glm::value_ptr(*selectedObject->getWireframeColor()), ImGuiColorEditFlags_NoLabel);
                    ImGui::TreePop();
                }
            }
            ImGui::End();
        }

        if (ImGui::Begin("Properties"))
        {
            if (selectedObject)
            {
               if (ImGui::TreeNode("Transformation"))
               {
                   DrawVec3Control("Scale", *selectedObject->getScaleFactor(), 0.05, -10.0f, 10.0f);
                   DrawVec3Control("Rotation", *selectedObject->getRotationFactor(), 10.0f, -360.0f, 360.0f);
                   DrawVec3Control("Translation", *selectedObject->getTranslationFactor(), 0.05, -max_float, max_float);

                   ImGui::TreePop();
               }
               if (ImGui::TreeNode("Bounding Box"))
               {
                   ImGui::Text("Color");
                   ImGui::SameLine();
                   ImGui::ColorEdit3("##Color", glm::value_ptr(*selectedObject->getBoxColor()));
                   ImGui::TreePop();
               }
               if (ImGui::TreeNode("Meshes Materials"))
               {
                   std::vector<Mesh> *meshes = selectedObject->getMeshes();
                   
                   for (int i = 0; i < meshes->size(); i++)
                   {
                       Mesh *m = &(*meshes)[i];

                       if (ImGui::TreeNode(m->getName().c_str()))
                       {
                           ImGui::Text("Show Mesh");
                           ImGui::SameLine();
                           ImGui::Checkbox("##showMesh", m->getShow());
                           ImGui::Text("Material Name: ");
                           ImGui::SameLine();
                           std::string mtlName = m->mtl->name;
                           ImGui::Text(mtlName.c_str());
                           ImGui::Text("Diffuse");
                           ImGui::SameLine();
                           ImGui::ColorEdit3("Diffuse", glm::value_ptr(*m->mtl->getDiffuse()), ImGuiColorEditFlags_NoLabel);
                           ImGui::TreePop();
                       }
                   }
                   ImGui::TreePop();
               }
            }
            else if (selectedCamera)
            {
                 glm::vec3* position = camera.getPosition();
                 DrawVec3Control("Position", *position, 0.05, -max_float, max_float);
            }

            ImGui::End();
        }
    }

    void setSelected(int selected)
    {
        this->selected = selected;
    }

    // Setup Dear ImGui context and style
	void init()
	{
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.FontDefault = io.Fonts->AddFontFromFileTTF("misc/fonts/Roboto-Regular.ttf", 16.0f);

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'docs/FONTS.md' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        //io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);
	}

    void deleteAllObjects()
    {
        selectedObject = nullptr;
        objects.clear();
        materials.clear();
    }

    void deleteSelected()
    {
        if (selectedObject)
        {
            std::vector<Object>::size_type i = selectedObject - &objects[0];
            assert(i < objects.size());
            std::vector<Object>::iterator it = objects.begin() + i;
            objects.erase(it);
            selectedObject = nullptr;
        }
    }

    Object loadObject()
    {
        std::vector<Mesh> meshes;
        std::string name, objPath, mtlPath;

        glm::vec3 normalize, scale, translation, angles;
        glm::vec3 wireframeColor, vertexColor, normalsColor, boxColor;
        glm::vec3 boxCenter, boxSize, vmin, vmax;
        bool useDepthTest, useCullFace, useMultisample, showWireframe, showVertices, showNormals;
        int pointSize;

        ss >> name;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> objPath;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> normalize.x >> normalize.y >> normalize.z;
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> scale.x >> scale.y >> scale.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> translation.x >> translation.y >> translation.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> angles.x >> angles.y >> angles.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> useDepthTest >> useCullFace >> useMultisample >> showWireframe >> showVertices >> showNormals; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> pointSize; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> wireframeColor.x >> wireframeColor.y >> wireframeColor.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vertexColor.x >> vertexColor.y >> vertexColor.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> normalsColor.x >> normalsColor.y >> normalsColor.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> boxColor.x >> boxColor.y >> boxColor.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> boxCenter.x >> boxCenter.y >> boxCenter.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> boxSize.x >> boxSize.y >> boxSize.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmin.x >> vmin.y >> vmin.z; 
        std::getline(infile, line);  ss.clear(); ss.str(line); ss >> prefix >> vmax.x >> vmax.y >> vmax.z;  

        mtlPath = objPath.substr(0, objPath.size() - 4) + ".mtl";
        mtlLoader.load((mtlPath).c_str());
        meshes = objloader.loadMeshes((objPath).c_str());

        return Object(meshes, objPath, name, normalize, scale, translation, angles, useDepthTest, useCullFace, useMultisample,
            showWireframe, showVertices, showNormals, pointSize, wireframeColor, vertexColor, normalsColor, boxColor,
            boxCenter, boxSize, vmin, vmax);
    }

	void render()
	{
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        if(ImGui::Begin("Scene"))
        {
            if (ImGui::TreeNode("Manage Scene"))
            {
                if (ImGui::Button("Load Scene"))
                    loadScene();
   
                if (ImGui::Button("Save Scene"))
                    saveScene();

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Manage Objects"))
            {
                if (ImGui::Button("Load Object"))
                    loadObj();
                if (ImGui::Button("Delete Selected Object"))
                    deleteSelected();
                if (ImGui::Button("Delete All Objects"))
                    deleteAllObjects();

                ImGui::TreePop();
            }           

            ImGui::Text("Background Color");
            ImGui::SameLine();
            ImGui::ColorEdit3("##bgColor", glm::value_ptr(*bgColor), ImGuiColorEditFlags_NoLabel);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        objectsWindow();

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

    void loadObj()
    {
        FILE* lIn;
        char const* objFileName, * lFilterPatterns[1] = { "*.obj"};
      
        objFileName = tinyfd_openFileDialog("Open", "", 1, lFilterPatterns, NULL, 0);

        if (!objFileName)
            return;
 
        std::string mtlFileName = objFileName;
        mtlFileName = mtlFileName.substr(0, mtlFileName.size() - 4) + ".mtl";

        mtlLoader.load(mtlFileName.c_str());

        /*
        for (int i = 0; i < materials.size(); i++)
        {
            std::cout << materials[i].name << " " << materials[i].getDiffuse()->x << " " << materials[i].getDiffuse()->y << " " << materials[i].getDiffuse()->z << std::endl;
        }*/

        objects.push_back(Object(objloader.loadMeshes(objFileName), objFileName));


        std::string objName = objFileName;
        std::size_t found = objName.find_last_of("/\\");
        objName = objName.substr(found + 1);
        objName = objName.substr(0, objName.size() - 4);
        objects.back().setName(objName);
        
        selected = objects.size();
        selectedObject = &objects.back();
    }

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

        std::ofstream outfile;
        outfile.open(lTheSaveFileName);
        outfile << "bg " << bgColor->x << " " << bgColor->y << " " << bgColor->z << "\n";

        for (int i = 0; i < objects.size(); i++)
        {
            objects[i].getInfo(outfile);
        }

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

        infile = std::ifstream(sceneFileName);


        std::getline(infile, line); std::cout << line << std::endl;  ss.clear(); ss.str(line);  ss >> prefix >> bgColor->x >> bgColor->y >> bgColor->z;
        
        while (std::getline(infile, line))
        {
            ss.clear(); ss.str(line);

            ss >> prefix;
            if (prefix == "o")
                objects.push_back(loadObject());
        }

    }

	void terminate()
	{
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
	}

private:
    GLFWwindow* window;
    ObjLoader objloader;
    MtlLoader mtlLoader;

    std::ifstream infile;
    std::string line, prefix;
    std::stringstream ss;

    // App state
    glm::vec3 *bgColor;

    // Our state
    int selected = -1;
    bool show_demo_window = true;
    bool show_another_window = false;

    // Limit values
    float max_float = std::numeric_limits<float>::max();
};