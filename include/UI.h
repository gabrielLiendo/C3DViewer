#pragma once
class UI
{
public:
    UI()
    {

    }

    UI(GLFWwindow* window)
    {
        this->window = window;
    }

    void objectsWindow()
    {
        if (ImGui::Begin("Objects"))
        {   
            static int selected = -1;
            int m = models.size(), index = 0;
            for (int i = 0; i < m; i++)
            {
                std::string label = "Object " + std::to_string(i + 1);
                if (ImGui::TreeNode(label.c_str()))
                {   
                    int n = models[i].meshes.size();
                    for (int j = 0; j < n; j++)
                    {
                        if (ImGui::Selectable(models[i].meshes[j].name.c_str(), selected == index))
                        {
                            selected = index;
                            selectedMesh = &models[i].meshes[j];
                        }
                        index++;
                    }

                    ImGui::TreePop();
                }
            }
                
            if (ImGui::Begin("Properties"))
            {
                if (ImGui::TreeNode("Transformation"))
                {
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Material"))
                {
                    ImGui::TreePop();
                }
                ImGui::End();
            }
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
        if(ImGui::Begin("Main", 0, ImGuiWindowFlags_MenuBar))
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
        mtlFileName = mtlFileName.substr(0, mtlFileName.size() - 3) + "mtl";

        mtlLoader.load(mtlFileName.c_str());

        /*for (int i = 0; i < materials.size(); i++)
        {
            std::cout << materials[i].name << " " << materials[i].kd.x << " " << materials[i].kd.y << " " << materials[i].kd.z << std::endl;
        }*/
        objloader.load(objFileName);
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

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};