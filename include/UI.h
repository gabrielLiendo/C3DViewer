#pragma once
class UI
{
public:
    UI() = default;

    UI(GLFWwindow* window)
    {
        this->window = window;
        init();
    }

    void render()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Draw the main window
        if (ImGui::Begin("Scene", 0, ImGuiWindowFlags_MenuBar))
        {
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Object"))
                {
                    if (ImGui::MenuItem("Load"))
                        loadersManager.loadObj(selected);
                    if (ImGui::MenuItem("Delete Selected"))
                        deleteSelected();
                    if (ImGui::MenuItem("Delete All"))
                        deleteAllObjects();
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Scene"))
                {
                    if (ImGui::MenuItem("Load Scene"))
                        loadersManager.loadScene();

                    if (ImGui::MenuItem("Save Scene"))
                        loadersManager.saveScene();

                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            if (ImGui::TreeNode("Background"))
            {
                ImGui::Text("Color");
                ImGui::SameLine();
                ImGui::ColorEdit3("##bgColor", glm::value_ptr(bgColor), ImGuiColorEditFlags_NoLabel);
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Framerate"))
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::TreePop();
            }

            ImGui::End();
        }

        // Draw the 'Rendering' and 'Properties' windows 
        if (ImGui::Begin("Rendering"))
        {
            if (selectedObject)
            {
                if (ImGui::TreeNodeEx("Options", ImGuiTreeNodeFlags_DefaultOpen))
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
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Vertices"))
                {
                    ImGui::Text("Show Vertices");
                    ImGui::SameLine();
                    ImGui::Checkbox("##Vertices", selectedObject->getVerticesBool());
                    ImGui::Text("Vertex size");
                    ImGui::SameLine();
                    ImGui::DragInt("##vertexSize", selectedObject->getPointSize(), 1, 1);
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
                if (ImGui::TreeNodeEx("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    DrawVec3Control("Scale", *selectedObject->getScaleFactor(), 0.05, -10.0f, 10.0f);
                    DrawVec3Control("Rotation", *selectedObject->getRotationFactor(), 10.0f, -360.0f, 360.0f);
                    DrawVec3Control("Translation", selectedObject->translation, 0.05, -max_float, max_float);

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
                    std::vector<Mesh>* meshes = selectedObject->getMeshes();

                    for (int i = 0; i < meshes->size(); i++)
                    {
                        Mesh* m = &(*meshes)[i];

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
                DrawVec3Control("Position", *camera.getPosition(), 0.05, -max_float, max_float);

            ImGui::End();
        }

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

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void terminate()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

private:
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
    }

    static void DrawVec3Control(const std::string label, glm::vec3& values, float v_speed, float v_min, float v_max)
    {
        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoPadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text(label.c_str());

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::Button("X");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::Button("Y");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(3);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::Button("Z");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::EndTable();
    }

    void setSelected(int selected)
    {
        this->selected = selected;
    }


    GLFWwindow* window;

    // Loaders
    LoadersManager loadersManager;

    // State
    int selected = -1;
    bool show_demo_window = true;

    // Limit values
    float max_float = std::numeric_limits<float>::max();
};