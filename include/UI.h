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
            static int selected = -1;


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
            }
            else if (selectedCamera)
            {
                 glm::vec3* position = camera.getPosition();
                 DrawVec3Control("Position", *position, 0.05, -max_float, max_float);
            }

             /*
             if (ImGui::TreeNode("Material"))
             {
                if (selectedObject)
                {
                    std::string mtlName = selectedObject->mtl->name;
                    ImGui::Text("Diffuse");
                    ImGui::SameLine();
                    ImGui::ColorEdit3("Diffuse", glm::value_ptr(*selectedMesh->mtl->getDiffuse()), ImGuiColorEditFlags_NoLabel);
                }
                ImGui::TreePop();
             }
             */
             ImGui::End();
        }
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

	void render()
	{
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        if(ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Load Object"))
                        loadObj();

                    ImGui::MenuItem("Load Scene");

                    ImGui::MenuItem("Save Scene");

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Help"))
                {
                    ImGui::MenuItem("Shortcuts", " ");
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
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

        objects.push_back(objloader.load(objFileName));

        std::string objName = objFileName;
        std::size_t found = objName.find_last_of("/\\");
        objName = objName.substr(found + 1);
        objName = objName.substr(0, objName.size() - 4);
        objects.back().setName(objName);
     
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

    // App state
    glm::vec3 *bgColor;

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;

    // Limit values
    float max_float = std::numeric_limits<float>::max();
};