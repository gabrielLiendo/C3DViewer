#pragma once
class C3DViewer
{
public:
    C3DViewer()
    {
        // Init GLFW
        glfwInit();

        // Set all the required options for GLFW
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // Create a GLFWwindow object that we can use for GLFW's functions
        window = glfwCreateWindow(width, height, "Proyecto 2 CG1 - UCV", nullptr, nullptr);
        glfwMakeContextCurrent(window);

        // Init Glad
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // OpenGL options
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glEnable(GL_MULTISAMPLE);
        glDepthFunc(GL_LESS);
        glViewport(0, 0, width, height);

        // Build and compile our shader programs
        basicShader = Shader("misc/shaders/vertex/basic.vs", "misc/shaders/fragment/flatColor.frag");
       
        normals_Shader = Shader("misc/shaders/vertex/passNormal.vs", "misc/shaders/fragment/flatColor.frag", "misc/shaders/geometry/normals.geom");
        circularVertex_shader = Shader("misc/shaders/vertex/basic.vs", "misc/shaders/fragment/pointColor.frag");

        ambientShader = Shader("misc/shaders/vertex/basic.vs", "misc/shaders/fragment/ambientLight.frag");
        phongShader = Shader("misc/shaders/vertex/ilumNormal.vs", "misc/shaders/fragment/phongLight.frag");

        // Initialize the view and projection matrices
        projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(width) / float(height), 0.1f, 100.0f);

        // Set functions and variables
        setCallbacks();

        // Create and initialize UI 
        ui = std::make_unique<UI>(window, &scene);
    }

    ~C3DViewer()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    // Set the required callback functions
    void setCallbacks()
    {
        C3DViewer* C3D_window = this;

        glfwSetWindowUserPointer(window, C3D_window);

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
                static_cast<C3DViewer*>(glfwGetWindowUserPointer(window))->mouse_callback(xpos, ypos);
        });

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) 
        {
                static_cast<C3DViewer*>(glfwGetWindowUserPointer(window))->key_callback(key, scancode, action, mode);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
        {
                static_cast<C3DViewer*>(glfwGetWindowUserPointer(window))->button_callback(button, action, mods);
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
                static_cast<C3DViewer*>(glfwGetWindowUserPointer(window))->resize_callback(window, width, height);
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
                static_cast<C3DViewer*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
        });
    }

    void resize_callback(GLFWwindow* window, int width, int height)
    {     
        this->width = width;
        this->height = height;

        glViewport(0, 0, width, height);

        projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(width) / float(height), 0.1f, 100.0f);

        // Re-render the scene because the current frame was drawn for the old resolution
        draw();
    }

    // Is called whenever a key is pressed/released via GLFW
    void key_callback(int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
            scene.deleteSelected();

        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
                keys[key] = true;
            else if (action == GLFW_RELEASE)
                keys[key] = false;
        }
    }

    void button_callback(int button, int action, int mods)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !io.WantCaptureMouse)
            processColorPicker = true;
    }

    void mouse_callback(double xpos, double ypos)
    { 
       ImGuiIO& io = ImGui::GetIO();

       if (!io.WantCaptureMouse)
       {    
           float xpos_ = (float)xpos, ypos_ = (float)ypos;

           if (firstMouse)
           {
               lastX = xpos_;
               lastY = height - ypos_;
               firstMouse = false;
           }
           else
           {
               float xoffset = xpos_ - lastX;
               float yoffset = (height - ypos_) - lastY;

               //std::cout << xoffset << " " << yoffset << std::endl;
               if (scene.selectedObject)
               {   
                   if(keys[GLFW_KEY_LEFT_CONTROL])
                   {
                        if(abs(xoffset) > abs(yoffset))
                            scene.selectedObject->addYRot(xoffset *  0.25f);
                        else if(abs(yoffset) > abs(xoffset))
                            scene.selectedObject->addXRot(yoffset * -0.25f);
                   }
                   else
                   {
                        scene.selectedObject->addXRot(yoffset * -0.25f);
                        scene.selectedObject->addYRot(xoffset *  0.25f);
                   }
               }
               else
                   scene.camera.changeDirection(xoffset, yoffset);

               lastX = xpos_;
               lastY = height - ypos_;
           }

           if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
               firstMouse = true;   
       }
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        ImGuiIO& io = ImGui::GetIO();

        if (!io.WantCaptureMouse)
            yScroll = yoffset;
        else
            yScroll = 0.0f;
    }

    double getYScroll()
    {
        double value = yScroll;
        yScroll = 0.0f;
        return value;
    }

    void do_movement(float delta)
    {
        double yScroll = getYScroll();

        // Move camera position
        if (keys[GLFW_KEY_W] || yScroll > 0)
            scene.camera.moveFrontBack(delta);
        else if (keys[GLFW_KEY_S] || yScroll < 0)
            scene.camera.moveFrontBack(-delta);
        else if (keys[GLFW_KEY_D])
            scene.camera.moveLeftRight(delta);
        else if (keys[GLFW_KEY_A])
            scene.camera.moveLeftRight(-delta);
    }

    void readPixel()
    {
        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char data[4];
        glReadPixels((int)lastX, (int)lastY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

        if (pickedID == 0x00ffffff) {
            scene.selectedObject = nullptr;
            ui->setSelected(-1);
        }
        else if (pickedID <= scene.objects.size()) {
            scene.selectedObject = scene.objects[pickedID];
            ui->setSelected(pickedID);
        }
    }

    void draw()
    {   
        // Data to pass to shaders
        glm::mat4 MVP;
        glm::mat4 objModel, boxModel, view;
        glm::vec3 boxColor, color;

        // Get view matrix from camera
        view = scene.camera.getView();

        // Calculate deltatime of current frame
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement(deltaTime);

        // Clear the colorbuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (processColorPicker)
        {
            basicShader.use();

            int n = (int)scene.objects.size();
            for (int i = 0; i < n; i++)
            {
                objModel = scene.objects[i]->getModelTransformation();

                // Pass the mvp matrix to the shader
                MVP = projection * view * objModel;

                basicShader.setMat4f("MVP", MVP);
                scene.objects[i]->drawFlatPicking(basicShader);
            }

            readPixel();
            processColorPicker = false;
        }

        // Clear the colorbuffer
        glClearColor(scene.bgColor.x, scene.bgColor.y, scene.bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use corresponding shader when setting uniforms/drawing objects
        if (scene.lightingModel == 0)
        {   
            ambientShader.use();
            ambientShader.setVec3f("gLight.color", *scene.light.getColor());
            ambientShader.setFloat("gLight.ambientIntensity", *scene.light.getAmbientIntensity());
        }
        else if(scene.lightingModel == 2)
        {
            phongShader.use();
            phongShader.setVec3f("view", scene.camera.position);
            phongShader.setVec3f("gLight.color", *scene.light.getColor());
            phongShader.setVec3f("gLight.position", *scene.light.getPosition());
            phongShader.setFloat("gLight.ambientIntensity", *scene.light.getAmbientIntensity());
            phongShader.setFloat("gLight.diffuseIntensity", *scene.light.getDiffuseIntensity());
        }
        
        // Render objects
        for (int i = 0; i < scene.objects.size(); i++)
        {
            objModel = scene.objects[i]->getModelTransformation();

            // Pass the mvp matrix to the shader
            MVP = projection * view * objModel;
           
            if (scene.lightingModel == 0)
            {
                ambientShader.use();
                ambientShader.setMat4f("MVP", MVP);
                scene.objects[i]->draw(AMBIENT, ambientShader);
            }
            else if(scene.lightingModel == 2)
            {
                phongShader.use();
                phongShader.setMat4f("MVP", MVP);
                phongShader.setMat4f("model", objModel);
                scene.objects[i]->draw(PHONG, phongShader);
            }
            
            if (*scene.objects[i]->getShowNormals())
            {
                // Use other shader to draw the normals
                normals_Shader.use();
                normals_Shader.setMat4f("MVP", MVP);
                normals_Shader.setVec3f("color", *scene.objects[i]->getNormalsColor());
                normals_Shader.setFloat("normalScale", scene.objects[i]->getScaleNormal());
                scene.objects[i]->drawVertices();
            }

            if (*scene.objects[i]->getShowVertices())
            {
                // Use other shader to draw the normals
                circularVertex_shader.use();      
                circularVertex_shader.setMat4f("MVP", MVP);
                circularVertex_shader.setVec3f("color", *scene.objects[i]->getVerticesColor());
                circularVertex_shader.setFloat("pointSize", float(*scene.objects[i]->getPointSize()));
                scene.objects[i]->drawVertices();
            }
        }

        if (scene.selectedObject)
        {
            boxModel = scene.selectedObject->getModelTransformation() * scene.selectedObject->getBoxModel();
            MVP = projection * view * boxModel;

            // Render Bounding Box with basic shader
            basicShader.use();
            basicShader.setVec3f("color", *scene.selectedObject->getBoxColor());
            basicShader.setMat4f("MVP", MVP);

            scene.selectedObject->drawBoundingBox();
        }

        // Render UI frame
        ui->render();

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    void run()
    {
        while (!glfwWindowShouldClose(window))
            draw();
    }

private:
    // Pointer to window
    GLFWwindow* window;
    
    // User Interface
    std::unique_ptr<UI> ui;

    // Group of scene variables
    Scene scene;

    // Window dimensions
    int width = 1600, height = 800;

    // Mouse Position
    float lastX = width / 2.0f;
    float lastY = height / 2.0f;
   
    bool processColorPicker = false;

    // Mouse Scroll 
    double yScroll = 0.0f;

    // Deltatime
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    // Keys and Mouse boolean state
    bool keys[1024] = {false};
    bool firstMouse = true;

    // Projection matriX
    glm::mat4 projection;

    //Shaders
    Shader basicShader, normals_Shader, circularVertex_shader, ambientShader, phongShader;
};