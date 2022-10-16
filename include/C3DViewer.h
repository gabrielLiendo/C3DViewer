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
        basic_shader = Shader("misc/shaders/vertex/basic.vs", "misc/shaders/fragment/flatColor.frag");
        lighting_shader = Shader("misc/shaders/vertex/ilum.vs", "misc/shaders/fragment/ilumColor.frag");
        normals_shader = Shader("misc/shaders/vertex/passNormal.vs", "misc/shaders/fragment/flatColor.frag", "misc/shaders/geometry/normals.geom");
        circularVertex_shader = Shader("misc/shaders/vertex/basic.vs", "misc/shaders/fragment/pointColor.frag");
        
        // Get the uniform locations
        mvpLoc_bs = glGetUniformLocation(basic_shader.Program, "MVP");
        colorLoc_bs = glGetUniformLocation(basic_shader.Program, "color");

        mvpLoc = glGetUniformLocation(lighting_shader.Program, "MVP");
        modelLoc = glGetUniformLocation(lighting_shader.Program, "model");
        viewLoc = glGetUniformLocation(lighting_shader.Program, "view");
        lightColorLoc = glGetUniformLocation(lighting_shader.Program, "gLight.color");
        lightAmbientIntensityLoc = glGetUniformLocation(lighting_shader.Program, "gLight.ambientIntensity");
        lightDiffuseIntensityLoc = glGetUniformLocation(lighting_shader.Program, "gLight.diffuseIntensity");
        lightPositionLoc = glGetUniformLocation(lighting_shader.Program, "gLight.position");
        mtlAmbientLoc = glGetUniformLocation(lighting_shader.Program, "gMaterial.ambientColor");
        mtlDiffuseLoc = glGetUniformLocation(lighting_shader.Program, "gMaterial.diffuseColor");
        mtlSpecularLoc = glGetUniformLocation(lighting_shader.Program, "gMaterial.specularColor");
       
        mvpLoc_ns = glGetUniformLocation(normals_shader.Program, "MVP");
        colorLoc_ns = glGetUniformLocation(normals_shader.Program, "color");
        scaleLoc_ns = glGetUniformLocation(normals_shader.Program, "normalScale");

        mvpLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "MVP");
        pointSizeLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "pointSize");
        colorLoc_cps = glGetUniformLocation(circularVertex_shader.Program, "color");

        // Set functions and variables
        setCallbacks();

        // Create and initialize UI 
        ui = UI(window, &scene);
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
           if (firstMouse)
           {
               lastX = xpos;
               lastY = height - ypos;
               firstMouse = false;
           }
           else
           {
               double xoffset = xpos - lastX;
               double yoffset = (height - ypos) - lastY;

               if (scene.selectedObject)
               {
                   scene.selectedObject->addXRot(- yoffset * 0.25);
                   scene.selectedObject->addYRot(xoffset * 0.25);
               }
               else
               {
                   scene.camera.changePosX( xoffset * 0.01);
                   scene.camera.changePosY( yoffset * 0.01);
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

    void do_movement(double delta)
    {
        double yScroll = getYScroll();

        // Camera controls
        if (keys[GLFW_KEY_UP] || yScroll > 0)
            scene.camera.changePosZ(-1.5f*delta);
        else if (keys[GLFW_KEY_DOWN] || yScroll < 0)
            scene.camera.changePosZ(1.5f*delta);
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
            ui.setSelected(-1);
        }
        else if (pickedID <= scene.objects.size()) {
            scene.selectedObject = &scene.objects[pickedID];
            ui.setSelected(pickedID);
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
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Define the viewport dimensions
        glViewport(0, 0, width, height);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement(deltaTime);

        // Create camera transformations
        view = glm::lookAt(*scene.camera.getPosition(), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(width) / float(height), 0.1f, 100.0f);

        // Clear the colorbuffer
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (processColorPicker)
        {
            basic_shader.Use();

            for (int i = 0; i < scene.objects.size(); i++)
            {
                objModel = scene.objects[i].getModelTransformation();

                // Pass the mvp matrix to the shader
                MVP = projection * view * objModel;
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

                scene.objects[i].drawFlatPicking(colorLoc_bs);
            }

            readPixel();
            processColorPicker = false;
        }

        // Clear the colorbuffer
        glClearColor(scene.bgColor.x, scene.bgColor.y, scene.bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use corresponding shader when setting uniforms/drawing objects
        if (scene.useLighting)
        {
            lighting_shader.Use();

            // Set lighting Uniforms
            glUniform3f(lightColorLoc, scene.light.getColor()->x, scene.light.getColor()->y, scene.light.getColor()->z);
            glUniform3f(lightPositionLoc, scene.light.getPosition()->x, scene.light.getPosition()->y, scene.light.getPosition()->z);
            glUniform1f(lightAmbientIntensityLoc, *scene.light.getAmbientIntensity());
            glUniform1f(lightDiffuseIntensityLoc, *scene.light.getDiffuseIntensity());
        }
        
        // Render objects
        for (int i = 0; i < scene.objects.size(); i++)
        {
            objModel = scene.objects[i].getModelTransformation();

            // Pass the mvp matrix to the shader
            MVP = projection * view * objModel;
           
            if (scene.useLighting)
            {
                lighting_shader.Use();
                glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objModel));
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                scene.objects[i].draw(true, mtlDiffuseLoc, mtlAmbientLoc, mtlSpecularLoc);
            }
            else
            {
                basic_shader.Use();
                glUniformMatrix4fv(mvpLoc_bs, 1, GL_FALSE, glm::value_ptr(MVP));
                scene.objects[i].draw(false, colorLoc_bs, 0, 0);
            }
                
            if (*scene.objects[i].getShowNormals())
            {
                // Use other shader to draw the normals
                normals_shader.Use();

                glUniformMatrix4fv(mvpLoc_ns, 1, GL_FALSE, glm::value_ptr(MVP));
                color = *scene.objects[i].getNormalsColor();
                glUniform3f(colorLoc_ns, color.x, color.y, color.z);
                glUniform1f(scaleLoc_ns, scene.objects[i].getScaleNormal());
                scene.objects[i].drawVertices();
            }

            if (*scene.objects[i].getShowVertices())
            {
                // Use other shader to draw the normals
                circularVertex_shader.Use();

                glUniformMatrix4fv(mvpLoc_cps, 1, GL_FALSE, glm::value_ptr(MVP));
                color = *scene.objects[i].getVerticesColor();
                glUniform3f(colorLoc_cps, color.x, color.y, color.z);
                glUniform1f(pointSizeLoc_cps, float(*scene.objects[i].getPointSize()));
                scene.objects[i].drawVertices();
            }
        }

        if (scene.selectedObject)
        {
            basic_shader.Use();

            boxModel = scene.selectedObject->getModelTransformation() * scene.selectedObject->getBoxModel();
            MVP = projection * view * boxModel;
            boxColor = *scene.selectedObject->getBoxColor();

            glUniform3f(colorLoc_bs, boxColor.x, boxColor.y, boxColor.z);
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

            scene.selectedObject->drawBoundingBox();
        }

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

    Scene scene;

    // Window dimensions
    const int width = 1600, height = 800;

    // Mouse Position
    double lastX = width / 2.0;
    double lastY = height / 2.0;
   
    bool processColorPicker = false;

    // Mouse Scroll 
    double yScroll = 0.0f;

    // Deltatime
    double deltaTime = 0.0f;	// Time between current frame and last frame
    double lastFrame = 0.0f; // Time of last frame

    // Keys and Mouse boolean state
    bool keys[1024] = {false};
    bool firstMouse = true;

    //Shaders
    Shader basic_shader, lighting_shader, normals_shader, circularVertex_shader;

    // Uniform locations
    int mvpLoc, modelLoc, viewLoc, lightColorLoc, lightAmbientIntensityLoc, lightDiffuseIntensityLoc, 
        lightPositionLoc, mtlAmbientLoc, mtlDiffuseLoc, mtlSpecularLoc;
    int mvpLoc_ns, colorLoc_ns, scaleLoc_ns;
    int mvpLoc_cps, colorLoc_cps, pointSizeLoc_cps;
    int mvpLoc_bs, colorLoc_bs;
};