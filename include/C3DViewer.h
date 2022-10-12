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
        glDepthFunc(GL_LESS);
        glViewport(0, 0, width, height);

        // Build and compile our shader programs
        basic_shader = Shader("shaders/vertex/basic.vs", "shaders/fragment/ilumColor.frag");
        normals_shader = Shader("shaders/vertex/passNormal.vs", "shaders/fragment/flatColor.frag", "shaders/geometry/normals.geom");
        circularVertex_shader = Shader("shaders/vertex/basic.vs", "shaders/fragment/pointColor.frag");
        picking_shader = Shader("shaders/vertex/basic.vs", "shaders/fragment/flatColor.frag");

        // Get the uniform locations
        mvpLoc = glGetUniformLocation(basic_shader.Program, "MVP");
        lightColorLoc = glGetUniformLocation(basic_shader.Program, "gLight.color");
        lightAmbientIntensityLoc = glGetUniformLocation(basic_shader.Program, "gLight.ambientIntensity");
        mtlAmbientLoc = glGetUniformLocation(basic_shader.Program, "gMaterial.ambientColor");
        mtlDiffuseLoc = glGetUniformLocation(basic_shader.Program, "gMaterial.diffuseColor");

        mvpLoc_ns = glGetUniformLocation(normals_shader.Program, "MVP");
        colorLoc_ns = glGetUniformLocation(normals_shader.Program, "color");
        scaleLoc_ns = glGetUniformLocation(normals_shader.Program, "normalScale");

        mvpLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "MVP");
        pointSizeLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "pointSize");
        colorLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "color");

        mvpLoc_ps = glGetUniformLocation(picking_shader.Program, "MVP");
        colorLoc_ps = glGetUniformLocation(picking_shader.Program, "color");

        // Set functions and variables
        setCallbacks();

        // Create and initialize UI 
        ui = UI(window, &sceneLayer, &camera);
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
        glViewport(0, 0, width, height);

        // Re-render the scene because the current frame was drawn for the old resolution
        draw();
    }

    // Is called whenever a key is pressed/released via GLFW
    void key_callback(int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
            sceneLayer.deleteSelected();

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

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !io.WantCaptureMouse) {
            std::cout << lastX << " " << lastY << std::endl;
            processColorPicker = true;
        }
    }

    void mouse_callback(double xpos, double ypos)
    { 
       ImGuiIO& io = ImGui::GetIO();

       if (!io.WantCaptureMouse)
       {
           if (firstMouse)
           {
               lastX = xpos;
               lastY = height - ypos;
               firstMouse = false;
           }
           else
           {
               float xoffset = xpos - lastX;
               float yoffset = (height - ypos) - lastY;

               if (sceneLayer.selectedObject)
               {
                   sceneLayer.selectedObject->addXRot(- yoffset * 0.25);
                   sceneLayer.selectedObject->addYRot(xoffset * 0.25);
               }
               else
               {
                   camera.changePosX(xoffset * 0.01);
                   camera.changePosY(yoffset * 0.01);
               }

               lastX = xpos;
               lastY = height - ypos;
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

    void do_movement(GLfloat delta)
    {
        double yScroll = getYScroll();

        // Camera controls
        if (keys[GLFW_KEY_UP] || yScroll > 0)
        {
            camera.changePosZ(-delta);
        }
        else if (keys[GLFW_KEY_DOWN] || yScroll < 0)
        {
            camera.changePosZ(delta);
        }
    }

    void readPixel()
    {
        glFlush();
        glFinish();

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char data[4];
        glReadPixels(lastX, lastY, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int pickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;

        if (pickedID == 0x00ffffff) {
            sceneLayer.selectedObject = nullptr;
        }
        else if (pickedID <= sceneLayer.objects.size()) {
            sceneLayer.selectedObject = &sceneLayer.objects[pickedID - 1];
            ui.setSelected(pickedID + 1);
        }
    }

    void draw()
    {   
        // Data to pass to shaders
        glm::mat4 MVP;
        glm::mat4 view, projection;
        glm::mat4 objModel, boxModel;
        glm::vec3 boxColor, color;

        // Calculate deltatime of current frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        // Define the viewport dimensions
        glViewport(0, 0, w, h);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement(deltaTime);

        // Create camera transformations
        view = glm::lookAt(*camera.getPosition(), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(w) / float(h), 0.1f, 100.0f);

        // Clear the colorbuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (processColorPicker)
        {
            picking_shader.Use();

            for (int i = 0; i < sceneLayer.objects.size(); i++)
            {
                objModel = sceneLayer.objects[i].getModelTransformation();

                // Pass the mvp matrix to the shader
                MVP = projection * view * objModel;
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

                sceneLayer.objects[i].drawFlatPicking(colorLoc_ps);
            }

            readPixel();
            basic_shader.Use();
            processColorPicker = false;
        }

        // Clear the colorbuffer
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use corresponding shader when setting uniforms/drawing objects
        basic_shader.Use();

        glUniform3f(lightColorLoc, sceneLayer.light.getColor()->x, sceneLayer.light.getColor()->y, sceneLayer.light.getColor()->z);
        glUniform1f(lightAmbientIntensityLoc, *sceneLayer.light.getAmbientIntensity());

        for (int i = 0; i < sceneLayer.objects.size(); i++)
        {
            objModel = sceneLayer.objects[i].getModelTransformation();

            // Pass the mvp matrix to the shader
            MVP = projection * view * objModel;
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

            sceneLayer.objects[i].draw(mtlDiffuseLoc, mtlAmbientLoc);

            if (*sceneLayer.objects[i].getShowNormals())
            {
                // Use other shader to draw the normals
                normals_shader.Use();

                glUniformMatrix4fv(mvpLoc_ns, 1, GL_FALSE, glm::value_ptr(MVP));
                color = *sceneLayer.objects[i].getNormalsColor();
                glUniform3f(colorLoc_ns, color.x, color.y, color.z);
                glUniform1f(scaleLoc_ns, sceneLayer.objects[i].getScaleNormal());
                sceneLayer.objects[i].drawVertices();

                // We start using our normal shader again
                basic_shader.Use();
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
            }

            if (*sceneLayer.objects[i].getShowVertices())
            {
                // Use other shader to draw the normals
                circularVertex_shader.Use();

                glUniformMatrix4fv(mvpLoc_cps, 1, GL_FALSE, glm::value_ptr(MVP));
                color = *sceneLayer.objects[i].getVerticesColor();
                glUniform3f(colorLoc_cps, color.x, color.y, color.z);
                glUniform1f(pointSizeLoc_cps, float(*sceneLayer.objects[i].getPointSize()));
                sceneLayer.objects[i].drawVertices();

                // We start using our normal shader again
                basic_shader.Use();
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
            }
        }

        /*
        if (sceneLayer.selectedObject)
        {
            boxModel = sceneLayer.selectedObject->getModelTransformation() * sceneLayer.selectedObject->getBoxModel();
            MVP = projection * view * boxModel;
            boxColor = *sceneLayer.selectedObject->getBoxColor();

            glUniform3f(colorLoc, boxColor.x, boxColor.y, boxColor.z);
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

            sceneLayer.selectedObject->drawBoundingBox();
        }*/

        // Render UI frame
        ui.render();

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    void run()
    {
        while (!glfwWindowShouldClose(window))
            draw();
    }

    void terminate()
    {
        ui.terminate();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

private:
    // Pointer to window
    GLFWwindow* window;

    // User Interface
    UI ui;

    SceneLayer sceneLayer;
  
    // Camera
    Camera camera;

    // Window dimensions
    const int width = 1600, height = 800;

    // Mouse Position
    float lastX = width / 2.0;
    float lastY = height / 2.0;
   
    bool processColorPicker = false;

    // Mouse Scroll 
    double yScroll = 0.0f;

    // Deltatime
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    // Keys and Mouse boolean state
    bool keys[1024] = {false};
    bool firstMouse = true;

    //Shaders
    Shader basic_shader, normals_shader, circularVertex_shader, picking_shader;

    // Uniform locations
    int mvpLoc, lightColorLoc, lightAmbientIntensityLoc, mtlAmbientLoc, mtlDiffuseLoc;
    int mvpLoc_ns, colorLoc_ns, scaleLoc_ns;
    int mvpLoc_cps, colorLoc_cps, pointSizeLoc_cps;
    int mvpLoc_ps, colorLoc_ps;
};