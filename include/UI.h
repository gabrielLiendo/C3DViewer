#pragma once
class UI
{
public:
    UI() = default;

    UI(GLFWwindow* window, Scene *scene)
    {
        this->window = window;
        this->scene = scene;
        this->loadersManager = LoadersManager(scene);

        init();
    }

    ~UI()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void setSelected(int selected){ this->selected = selected;}

    void render()
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Draw the frame Windows and Dockspace
        renderDockSpace();
        renderSceneConfigWindows();
        renderObjectConfigWindows();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

private:
    // Setup Dear ImGui context and style
    void init()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.FontDefault = io.Fonts->AddFontFromFileTTF("misc/fonts/Roboto-Regular.ttf", 16.0f);

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);

        auto& colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.0078f, 0.031f, 0.058f, 0.941f };
        colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.039f, 0.094f, 0.176f, 1.0f };
 
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");
    }

    // Create the docking enviorment
    void renderDockSpace()
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags host_window_flags = 0;
        host_window_flags |= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
        host_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            host_window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Dockspace", nullptr, host_window_flags);
        ImGui::PopStyleVar(3);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);

        // Create Menu Bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Load Object"))
                    loadersManager.loadObj(selected);
                if (ImGui::MenuItem("Load Scene"))
                    loadersManager.loadScene();
                if (ImGui::MenuItem("Save Scene"))
                    loadersManager.saveScene();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Delete Selected", "Backspace"))
                    scene->deleteSelected();
                if (ImGui::MenuItem("Delete All", "X"))
                    openPopupDelete = true;
                   
                ImGui::EndMenu();
            } 
            if (openPopupDelete == true)
            {
                ImGui::OpenPopup("Delete All?");
                openPopupDelete = false;
            }

            if (ImGui::BeginPopupModal("Delete All?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("All the objects will be deleted.\nThis operation cannot be undone!\n\n");
                ImGui::Separator();

                if (ImGui::Button("OK", ImVec2(120, 0))) 
                {   
                    scene->deleteAllObjects();
                    ImGui::CloseCurrentPopup(); 
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
                ImGui::EndPopup();
            }
           
        ImGui::EndMenuBar();
        }
        ImGui::End();
    }

    // Draw the 'Lighting', 'Scene', 'Camera', 'Objects' and 'Materials' windows 
    void renderSceneConfigWindows()
    {
        ImGui::Begin("Lighting");
        {   
            if (ImGui::TreeNodeEx("Configuration##Ligthing", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Combo("Model", &scene->lightingModel, "Ambient\0Lambert\0Phong\0\0");
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Color##Ligthing"))
            {
                ImGui::ColorEdit3("##LightColor", glm::value_ptr(*scene->light.getColor()));
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Position##Ligthing"))
            {
                Vec3Control(" ", *scene->light.getPosition(), 0.05f, -max_float, max_float);
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Intesity##Ligthing"))
            {
                ImGui::DragFloat("Ambient", scene->light.getAmbientIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::DragFloat("Diffuse", scene->light.getDiffuseIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::TreePop();
            }

            ImGui::End();
        }

        ImGui::Begin("Camera");
        {
            if (ImGui::TreeNodeEx("Control##Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                Camera *camera = &scene->camera;
                Vec3Control("Position", scene->camera.position, 0.05f, -max_float, max_float, &Camera::updateView, camera);
                Vec3Control("Front", scene->camera.front, 0.05f, -max_float, max_float, &Camera::updateView, camera);
                Vec3Control("Up", scene->camera.up, 0.05f, -max_float, max_float, &Camera::updateView, camera);
            
                ImGui::TreePop();
            }

            ImGui::End();
        }

        ImGui::Begin("Scene");
        {
            if (ImGui::TreeNodeEx("Background", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("Color##bgColor", glm::value_ptr(scene->bgColor));
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Rendering Options", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::Checkbox("Enable Z-buffer##z-buffer", &scene->useDepthTest))
                    scene->setDepthTest();

                if (ImGui::Checkbox("Enable Back-face Culling##backCulling", &scene->useCullFace))
                    scene->setCullFace();

                if (ImGui::Checkbox("Enable Antialiasing##antialiasing", &scene->useMultisample))
                    scene->setMultisample();

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Framerate"))
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::TreePop();
            }

            ImGui::End();
        }

        ImGui::Begin("Materials");
        {
            size_t n = scene->materials.size();
            std::shared_ptr<Material> mtl = scene->materials[0];

            if (ImGui::TreeNode(mtl->getName()->c_str()))
            {
                ImGui::ColorEdit3("Diffuse##Material", glm::value_ptr(*mtl->getDiffuse()));
                ImGui::ColorEdit3("Ambient##Material", glm::value_ptr(*mtl->getAmbient()));
                ImGui::ColorEdit3("Specular##Material", glm::value_ptr(*mtl->getSpecular()));
                ImGui::TreePop();
            }

            for (size_t i = 1; i < n; i++)
            {
                mtl = scene->materials[i];
                if (ImGui::TreeNodeEx(mtl->getName()->c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::ColorEdit3("Diffuse##Material", glm::value_ptr(*mtl->getDiffuse()));
                    ImGui::ColorEdit3("Ambient##Material", glm::value_ptr(*mtl->getAmbient()));
                    ImGui::ColorEdit3("Specular##Material", glm::value_ptr(*mtl->getSpecular()));
                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }

        ImGui::Begin("Objects");
        {
            int m = (int)scene->objects.size();
           
            for (int i = 0; i < m; i++)
            {
                std::shared_ptr<Object> obj = scene->objects[i];
                if (ImGui::Selectable(obj->name.c_str(), selected == i))
                {
                    selected = i;
                    scene->setSelectedObject(obj);
                }
            }

            ImGui::End();
        }
    }

    // Draw the 'Properties' and 'Meshes' windows 
    void renderObjectConfigWindows()
    {
        std::shared_ptr<Object> selectedObject = scene->selectedObject;

        ImGui::Begin("Meshes");
        {
            if (selectedObject)
            {
                std::vector<Mesh>* meshes = selectedObject->getMeshes();
                size_t n = meshes->size();

                for (size_t i = 0; i < n; i++)
                {
                    Mesh* m = &(*meshes)[i];

                    if (ImGui::TreeNode(m->getName().c_str()))
                    {
                        ImGui::Checkbox("Show Mesh##showMesh", m->getShow());
                        if (ImGui::TreeNode("Material##MeshMaterial"))
                        {
                            ImGui::Text("Material Name: ");
                            ImGui::SameLine();
                            std::string mtlName = *m->mtl->getName();
                            ImGui::Text(mtlName.c_str());
                            ImGui::ColorEdit3("Diffuse##MeshDiffuse", glm::value_ptr(*m->mtl->getDiffuse()));
                            ImGui::TreePop();
                        }
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::End();
        }

        ImGui::Begin("Properties");
        {
            if (selectedObject)
            {
                if (ImGui::TreeNodeEx("Model Transformation", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    Vec3Control("Scale", selectedObject->scale, 0.05f, -10.0f, 10.0f, &Object::setScaleMat, selectedObject);
                    Vec3Control("Rotation", selectedObject->angles, 5.0f, -360.0f, 360.0f, &Object::setRotationsMats, selectedObject);
                    Vec3Control("Translation", selectedObject->translation, 0.05f, -max_float, max_float, &Object::setTranslationMat, selectedObject);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Vertices", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Show Vertices##Vertices", selectedObject->getShowVertices());
                    ImGui::DragInt("Vertex size##vertexSize", selectedObject->getPointSize(), 1, 1);
                    ImGui::ColorEdit3("Color##VerticesColor", glm::value_ptr(*selectedObject->getVerticesColor()));
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Normals", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Show Normals##VNormals", selectedObject->getShowNormals());
                    ImGui::ColorEdit3("Color##NormalsColor", glm::value_ptr(*selectedObject->getNormalsColor()));
                    ImGui::TreePop();
                }

                if (ImGui::TreeNodeEx("Wireframe", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Checkbox("Show Wireframe##Wireframe", selectedObject->getWireframeBool());
                    ImGui::ColorEdit3("Color##WireColor", glm::value_ptr(*selectedObject->getWireframeColor()));
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Bounding Box"))
                {
                    ImGui::ColorEdit3("Color##ColorBoundingBox", glm::value_ptr(*selectedObject->getBoxColor()));
                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }
    }

    static void Vec3Control(const std::string label, glm::vec3& values, float v_speed, float v_min, float v_max)
    {
        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoPadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::Button("X");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::Button("Y");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);
            
        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::Button("Z");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp);

        ImGui::TableSetColumnIndex(3);
        ImGui::Text(label.c_str());

        ImGui::EndTable();
    }

    static void Vec3Control(const std::string label, glm::vec3& values, float v_speed, float v_min, float v_max, void (Object::*updateMat)(), std::shared_ptr<Object> selectedObject)
    {
        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoPadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::Button("X");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##X", &values.x, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (&(*selectedObject)->*updateMat)();

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::Button("Y");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##Y", &values.y, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (&(*selectedObject)->*updateMat)();
            
        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::Button("Z");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##Z", &values.z, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (&(*selectedObject)->*updateMat)();

        ImGui::TableSetColumnIndex(3);
        ImGui::Text(label.c_str());

        ImGui::EndTable();
    }

    static void Vec3Control(const std::string label, glm::vec3& values, float v_speed, float v_min, float v_max, void (Camera::*updateMat)(), Camera* camera)
    {
        ImGui::BeginTable(label.c_str(), 4, ImGuiTableFlags_NoPadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::Button("X");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##X", &values.x, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (camera->*updateMat)();

        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::Button("Y");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##Y", &values.y, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (camera->*updateMat)();
            
        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::Button("Z");
        ImGui::PopStyleColor(1);
        ImGui::SameLine();
        if(ImGui::DragFloat("##Z", &values.z, v_speed, v_min, v_max, "%.2f", ImGuiSliderFlags_AlwaysClamp))
            (camera->*updateMat)();

        ImGui::TableSetColumnIndex(3);
        ImGui::Text(label.c_str());

        ImGui::EndTable();
    }

    // References
    GLFWwindow* window;
    Scene* scene;

    // Loaders
    LoadersManager loadersManager;

    // State
    int selected = -1;
    bool openPopupDelete = false;
    bool show_demo_window = true;

    // Slider limit values
    float max_float = std::numeric_limits<float>::max();
};