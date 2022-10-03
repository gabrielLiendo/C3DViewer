#pragma once

class MyGlWindow
{
public:
    MyGlWindow()
    {
        // Init GLFW
        glfwInit();
        // Set all the required options for GLFW, (GLSL version required = 3.0)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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

        // Set functions and variables
        setCallbacks();
        setRenderVariables();

        // Create and initialize UI 
        ui = UI(window);
    }

    // Set the required callback functions
    void setCallbacks()
    {
        MyGlWindow* myWindow = this;

        glfwSetWindowUserPointer(window, myWindow);

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
                static_cast<MyGlWindow*>(glfwGetWindowUserPointer(window))->mouse_callback(xpos, ypos);
        });

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode) 
        {
                static_cast<MyGlWindow*>(glfwGetWindowUserPointer(window))->key_callback(key, scancode, action, mode);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
        {
                static_cast<MyGlWindow*>(glfwGetWindowUserPointer(window))->button_callback(button, action, mods);
        });

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
                static_cast<MyGlWindow*>(glfwGetWindowUserPointer(window))->resize_callback(window, width, height);
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
                static_cast<MyGlWindow*>(glfwGetWindowUserPointer(window))->scroll_callback(window, xoffset, yoffset);
        });
    }

    void setRenderVariables()
    {
        // Build and compile our shader program
        basic_shader = Shader("shaders/color_shader.vs", "shaders/color_shader.frag");
        normals_shader = Shader("shaders/normals_shader.vs", "shaders/normals_shader.frag", "shaders/normals_shader.geom");

        // Get the uniform locations
        modelLoc = glGetUniformLocation(basic_shader.Program, "model");
        viewLoc = glGetUniformLocation(basic_shader.Program, "view");
        projLoc = glGetUniformLocation(basic_shader.Program, "projection");
        colorLoc = glGetUniformLocation(basic_shader.Program, "diffuseColor");

        modelLoc_ns = glGetUniformLocation(normals_shader.Program, "model");
        viewLoc_ns = glGetUniformLocation(normals_shader.Program, "view");
        projLoc_ns = glGetUniformLocation(normals_shader.Program, "projection");
        colorLoc_ns = glGetUniformLocation(normals_shader.Program, "normalsColor");
        scaleLoc_ns = glGetUniformLocation(normals_shader.Program, "normalScale");
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
            deleteSelected();

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
        /*
        ImGuiIO& io = ImGui::GetIO();

        if (lbutton_down && !io.WantCaptureMouse) {
            float intersection_distance;
            glm::vec3 ray_origin, ray_direction;

            ScreenPosToWorldRay(ray_origin, ray_direction);

            std::vector<ObjectDistance> intersectedObjects;

            for (int i = 0; i < objects.size(); i++)
            {
                if (objects[i].TestRayOBBIntersection(ray_origin, ray_direction, intersection_distance))
                    intersectedObjects.push_back(ObjectDistance({ &objects[i] , intersection_distance, i}));
            }

            if (intersectedObjects.size() > 0)
            {
                ObjectDistance closestObj = intersectedObjects[0];

                float distance, min = closestObj.intersectionDistance;
                for (int i = 1; i < intersectedObjects.size(); i++)
                {
                    distance = intersectedObjects[i].intersectionDistance;
                    std::cout << intersectedObjects[i].obj->getName() << " " << distance << std::endl;
                    if (distance < min)
                    {
                        min = distance;
                        closestObj = intersectedObjects[i];
                    }
                }

                selectedObject = closestObj.obj;
                ui.setSelected(closestObj.index + 1);
            }
            else {
                selectedObject = nullptr;
            }
        }*/
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

               if (selectedObject)
               {
                   selectedObject->addXRot(- yoffset * 0.25);
                   selectedObject->addYRot(xoffset * 0.25);
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
        std::cout << yoffset << std::endl;
        yScroll = yoffset;
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

    void ScreenPosToWorldRay(glm::vec3& ray_origin, glm::vec3& ray_direction) 
    {
        // Ouput : Origin of the ray. 
        // Ouput : Direction, in world space, of the ray that goes "through" the mouse.

        // Transform to Normalized Device Coordinates
        float normalizedX = (2.0f*(float)lastX / (float)width) - 1.0f;
        float normalizedY = (2.0f*(float)lastY / (float)height) - 1.0f;

        // Transform to Homogeneous Clip Coordinates
        glm::vec4 lRayStart_NDC(normalizedX, normalizedY, -1.0, 1.0f);
        glm::vec4 lRayEnd_NDC(normalizedX, normalizedY, 0.0, 1.0f);

        // Inverse(ProjectionMatrix) goes from NDC to Camera Space.
        // Inverse(ViewMatrix) goes from Camera Space to World Space.
        glm::mat4 M = glm::inverse(projection * view);
        glm::vec4 lRayStart_world = M * lRayStart_NDC;     lRayStart_world/=lRayStart_world.w;
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ;     lRayEnd_world  /=lRayEnd_world.w;

        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);

        ray_origin = glm::vec3(lRayStart_world);
        ray_direction = glm::normalize(lRayDir_world);
    }

    void draw()
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);

        // Define the viewport dimensions
        glViewport(0, 0, w, h);

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement(deltaTime);

        // Clear the colorbuffer
        glClearColor(bgColor.x, bgColor.y, bgColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create camera transformations
        /*
        glm::mat4 CameraMatrix = glm::lookAt(
             cameraPosition, // the position of your camera, in world space
             cameraTarget,   // where you want to look at, in world space
             upVector        // probably glm::vec3(0,1,0), but (0,-1,0) would make you looking upside-down, which can be great too

        glm::mat4 projectionMatrix = glm::perspective(
            glm::radians(FoV), // The vertical Field of View, in radians: the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
            4.0f / 3.0f,       // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960, sounds familiar ?
            0.1f,              // Near clipping plane. Keep as big as possible, or you'll get precision issues.
            100.0f             // Far clipping plane. Keep as little as possible.
        */

        view = glm::lookAt(*camera.getPosition(), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(w) / float(h), 0.1f, 100.0f);

        // Use cooresponding shader when setting uniforms/drawing objects
        basic_shader.Use();

        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        for (int i = 0; i < objects.size(); i++)
        {
            objModel = objects[i].getModelTransformation();
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objModel));

            objects[i].draw(colorLoc);

            if (*objects[i].getNormalsBool())
            {
                normals_shader.Use();

                // Pass the matrices to the shader
                glUniformMatrix4fv(modelLoc_ns, 1, GL_FALSE, glm::value_ptr(objModel));
                glUniformMatrix4fv(viewLoc_ns, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(projLoc_ns, 1, GL_FALSE, glm::value_ptr(projection));
                normalsColor = *objects[i].getNormalsColor();
                glUniform3f(colorLoc_ns, normalsColor.x, normalsColor.y, normalsColor.z);
                glUniform1f(scaleLoc_ns, objects[i].getScaleNormal());
                objects[i].drawNormals();

                basic_shader.Use();

                // Pass the matrices to the shader
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objModel));
            }
        }

        if (selectedObject)
        {
            boxModel = selectedObject->getModelTransformation() * selectedObject->getBoxModel();
            boxColor = *selectedObject->getBoxColor();
            glUniform3f(colorLoc, boxColor.x, boxColor.y, boxColor.z);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(boxModel));
            selectedObject->drawBoundingBox();
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
    UI ui;
  
    // Window dimensions
    const int width = 1600, height = 800;

    // Mouse Position
    float xoffset = 0,  yoffset = 0; 
    float lastX = width / 2.0;
    float lastY = height / 2.0;
   
    // Deltatime
    float deltaTime = 0.0f;	// Time between current frame and last frame
    float lastFrame = 0.0f; // Time of last frame

    float angle = 0.0f;

    bool firstMouse = true;
    bool keys[1024];

    double yScroll = 0.0f;

    glm::mat4 view, projection;

    //Shaders
    Shader basic_shader, normals_shader;

    // Uniform locations
    int modelLoc, viewLoc, projLoc, colorLoc;
    int modelLoc_ns,viewLoc_ns, projLoc_ns, colorLoc_ns, scaleLoc_ns;

    glm::mat4 objModel, boxModel;
    glm::vec3 boxColor, normalsColor;

    // Limit values
    float max_float = std::numeric_limits<float>::max();
};
