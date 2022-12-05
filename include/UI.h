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

    bool getCombination(Colors color, Origin origin)  
    {   
        int combinationIdx;
        switch(color)
        {
            case AMBIENT_COLOR:
                combinationIdx = ambientCombinationIdx;
                break;
            case DIFFUSE_COLOR:
                combinationIdx = diffuseCombinationIdx;
                break;
            case SPECULAR_COLOR:
                combinationIdx = specularCombinationIdx;
                break;
        }

        if(origin==MATERIAL)
            return !(combinationIdx == 1);
        else // origin == TEXTURE
            return !(combinationIdx == 0);
    }

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
        ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);
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

    // Draw the 'Lighting', 'Scene', 'Camera', and'Objects' windows 
    void renderSceneConfigWindows()
    {
        ImGui::Begin("Lighting");
        {   
            
            if (ImGui::TreeNodeEx("Configuration##Ligthing", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Combo("Lighting", &scene->lightingModel, "Ambient\0Lambert\0Phong\0\0");
                ImGui::Combo("Shading", &scene->shadingModel, "Flat\0Gourand\0Phong\0\0");
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Ambiental##Ligthing", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::ColorEdit3("Color##LightColor", glm::value_ptr(scene->ambientColor));
                ImGui::DragFloat("Intensity", &scene->ambientIntensity, 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Lights##Ligthing", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (ImGui::TreeNode("New Light##Ligthing"))
                {   
                    ImGui::Combo("Lighting", &typeNewLight, "Point Light\0Directional Light\0\0");

                    if(typeNewLight == 0)
                    {
                        PointLightControl(scene->newLight);
                        if(ImGui::Button("Add Light"))
                            scene->addNewLight(POINT);
                    }
                    else
                    {
                        DirectionalLightControl(scene->newLight);
                        if(ImGui::Button("Add Light"))
                            scene->addNewLight(DIRECTIONAL);
                    }

                    ImGui::TreePop();
                }
                if (ImGui::TreeNodeEx("Active Lights##Ligthing", ImGuiTreeNodeFlags_DefaultOpen))
                {   
                    int n = scene->lights.size();
                    for(int i = 0; i < n; i++)
                    {   
                        std::string lightID;
                        if(scene->lights[i].isDirectional)
                        {
                            std::string lightID = "Directional Light##" + std::to_string(i+1);
                            if (ImGui::TreeNode(lightID.c_str()))
                            {
                                DirectionalLightControl(scene->lights[i]);
                                if(ImGui::Button("Remove Light"))
                                    scene->removeLight(DIRECTIONAL, i);

                                ImGui::TreePop();
                            }
                        }
                        else if(scene->lights[i].isPoint)
                        {
                            std::string lightID = "Point Light##" + std::to_string(i+1);
                            if (ImGui::TreeNode(lightID.c_str()))
                            {
                                PointLightControl(scene->lights[i]);
                                if(ImGui::Button("Remove Light"))
                                    scene->removeLight(POINT, i);
                                    
                                ImGui::TreePop();
                            }
                        }
                    }
                    ImGui::TreePop();
                }

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

            if (ImGui::TreeNodeEx("Fragment Colors", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ColorCombinationControl("Ambient", ambientCombinationIdx);
                ColorCombinationControl("Diffuse", diffuseCombinationIdx);
                ColorCombinationControl("Specular", specularCombinationIdx);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Framerate"))
            {
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                ImGui::TreePop();
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
                int n = (int)meshes->size();

                for (int i = 0; i < n; i++)
                {
                    Mesh* mesh = &(*meshes)[i];
                    std::shared_ptr<Material> mtl = mesh->mtl;

                    std::string meshID = mesh->getName() + "##" +std::to_string(i);
                    if (ImGui::TreeNode(meshID.c_str()))
                    {   
                        ImGui::Checkbox("Show Mesh", mesh->getShow());
                        if (ImGui::TreeNode("Material"))
                        {
                            ImGui::ColorEdit3("Diffuse", glm::value_ptr(*mtl->getDiffuse()));
                            ImGui::ColorEdit3("Ambient", glm::value_ptr(*mtl->getAmbient()));
                            ImGui::ColorEdit3("Specular", glm::value_ptr(*mtl->getSpecular()));
                            ImGui::DragFloat("Shininess", mtl->getShininess(), 1.0f, 0.5f, 128.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
                            
                            TextureMapNode("Ambient Map", mtl, mtl->kaMap, AMBIENT_MAP);
                            TextureMapNode("Diffuse Map", mtl, mtl->kdMap, DIFFUSE_MAP);
                            TextureMapNode("Specular Map", mtl, mtl->ksMap, SPECULAR_MAP);

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
                    Vec3Control("Scale", selectedObject->scale, 0.5f, -30.0f, 30.0f, &Object::setScaleMat, selectedObject);
                    Vec3Control("Rotation", selectedObject->angles, 3.0f, -360.0f, 360.0f, &Object::setRotationsMats, selectedObject);
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

                if (ImGui::TreeNodeEx("Texture Coordinates", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    ImGui::Combo("Mapping", &typeMapping, "Planar XY\0Spherical\0\0");
                    if(ImGui::Button("Generate Coordinates"))
                    {
                        if(typeMapping==0)
                            selectedObject->setPlanarTextCoords();
                        else if(typeMapping==1)
                            selectedObject->setSphericalTextCoords();
                    }
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

    void PointLightControl(Light &light)
    {
        Vec3Control("Position", *light.getDirection(), 0.05f, -max_float, max_float);
        ImGui::ColorEdit3("Diffuse##DC", glm::value_ptr(*light.getDiffuseColor()));
        ImGui::DragFloat("Intensity##DC", light.getDiffuseIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::ColorEdit3("Specular##SC", glm::value_ptr(*light.getSpecularColor()));
        ImGui::DragFloat("Intensity##SC", light.getSpecularIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);

        if (ImGui::TreeNode("Attenuation"))
        {
            ImGui::DragFloat("A", light.getConstantComponent(), 0.01f, 0.01f, 2.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("B", light.getLinearComponent(), 0.01f, 0.01f, 2.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::DragFloat("C", light.getQuadraticComponent(), 0.01f, 0.001f, 2.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
            ImGui::TreePop();
        }
    }

    void DirectionalLightControl(Light &light)
    {
        Vec3Control("Direction", *light.getDirection(), 0.05f, -max_float, max_float);
        ImGui::ColorEdit3("Diffuse##DC", glm::value_ptr(*light.getDiffuseColor()));
        ImGui::DragFloat("Intensity##DC", light.getDiffuseIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
        ImGui::ColorEdit3("Specular##SC", glm::value_ptr(*light.getSpecularColor()));
        ImGui::DragFloat("Intensity##SC", light.getSpecularIntensity(), 0.01f, 0.0f, 1.0f, "%.02f", ImGuiSliderFlags_AlwaysClamp);
    }
    
    void TextureMapNode(const char* nodeName, std::shared_ptr<Material> mtl, std::shared_ptr<Texture> textureMap, TextureMap mapType)
    {
        if (ImGui::TreeNode(nodeName))
        {
            if(textureMap)
                TextureControl(mtl, mapType, textureMap->getName(), textureMap->getIdentifier(), 45);
            else{
                if(ImGui::Button("Add Texture Map"))
                {
                    std::shared_ptr<Texture> newTexture = loadersManager.loadTexture();
                    mtl->setTextureMap(newTexture, mapType);
                }   
            }
            ImGui::TreePop();
        }
    }

    void ColorCombinationControl(std::string colorInput, int &current_combination_idx)
    {
        const char* combo_preview_value = combinations[current_combination_idx]; 

        if (ImGui::BeginCombo(colorInput.c_str(), combo_preview_value))
        {
            for (int n = 0; n < 3; n++)
            {
                const bool is_selected = (current_combination_idx == n);
                if (ImGui::Selectable(combinations[n], is_selected))
                    current_combination_idx= n;

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

    }

    void TextureControl(std::shared_ptr<Material> mtl, TextureMap mapType, const std::string *name, GLuint texture_id, int texSize)
    {   
        ImGui::BeginTable((*name).c_str(), 2, ImGuiTableFlags_PadOuterX);
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::Text((*name).c_str());
        if(ImGui::Button("Change"))
        {
            std::shared_ptr<Texture> newTexture = loadersManager.loadTexture();
            mtl->setTextureMap(newTexture, mapType);
        };
        ImGui::SameLine();
        if(ImGui::Button("Delete"))
        {
            mtl->removeTextureMap(mapType);
        };

        ImGui::TableSetColumnIndex(1);
        ImGui::SetCursorPosX(ImGui::GetCursorPos().x + (ImGui::GetContentRegionAvail().x - texSize) * 0.5f);
        ImGui::Image((void*)(intptr_t)texture_id, ImVec2(texSize, texSize));
                
        ImGui::EndTable();  
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

    int typeNewLight = 0; 
    int typeMapping = 0; 
    int ambientCombinationIdx = 1, specularCombinationIdx = 1, diffuseCombinationIdx = 1;
    char* combinations[3] = { "Only use material color", "Only use texture map color", "Combine both colors" };

    // Slider limit values
    float max_float = std::numeric_limits<float>::max();
};