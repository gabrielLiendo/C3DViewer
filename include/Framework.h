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
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        // Create a GLFWwindow object that we can use for GLFW's functions
        window = glfwCreateWindow(width, height, "Proyecto 2 CG1 - UCV", nullptr, nullptr);
        glfwMakeContextCurrent(window);

        // Init Glad
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // OpenGL options
        glEnable(GL_DEPTH_TEST);

        setCallbacks();

        
        bgColor = glm::vec3(0.0f, 0.0f, 0.0f);
        ui = UI(window, &bgColor);
        ui.init();
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
    }

    // Is called whenever a key is pressed/released via GLFW
    void key_callback(int key, int scancode, int action, int mode)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
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
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (GLFW_PRESS == action)
                lbutton_down = true;
            else if (GLFW_RELEASE == action)
                lbutton_down = false;
        }

        ImGuiIO& io = ImGui::GetIO();

        if (lbutton_down && !io.WantCaptureMouse) {
            // do your drag here
            std::cout << "CLICK " << lastX << " " << lastY << std::endl;

            glm::mat4 M = glm::inverse(view);
            glm::vec4 ex = M * glm::vec4(((float)lastX / (float)width - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
                ((float)lastY / (float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
                -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
                1.0f
            );

            double normalizedX = -1.0 + 2.0 * (double)lastX / width;
            double normalizedY = -(1.0 - 2.0 * (double)lastY / height);

            //std::cout << ex.x << " " << ex.y << std::endl;

            float intersection_distance;
            glm::vec3 ray_origin, ray_direction;
            ScreenPosToWorldRay(lastX, lastY, ray_origin, ray_direction);

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
        }
    }

    void mouse_callback(double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = height - ypos - 1;
            firstMouse = false;
        }
       
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
        {
            return;
        }

        /*
        if (!selectedObject)
        { 
            double xoffset = xpos - lastX;
            double yoffset = lastY - ypos ;

            std::cout << xpos << " " << lastX << " " << xoffset  << std::endl;
            std::cout << ypos  << " " << lastY << " " << yoffset  << std::endl;

            lastX = xpos;
            lastY = height - ypos - 1;

            //object_rotation.y += xoffset; // If i use yoffset the rotation flips
            //object_rotation.x += yoffset;

            //rotation_angle += (xoffset + yoffset) * 0.25f;

           

            //camera.changePosX(xoffset*0.25);
            //camera.changePosY(yoffset*0.25);

            std::cout << "BEFORE: " << camera.position.x << " " << camera.position.y << " " << std::endl;

            camera.changePosX(xoffset*0.25);
            camera.changePosY(yoffset*0.25);

            std::cout << "After: " << camera.position.x << " " << camera.position.y << " " << std::endl;
            
        }

        GLfloat xoffset = xpos - lastX;
        // Reversed since y-coordinates go from bottom to left
        GLfloat yoffset = lastY - ypos;

        */
        

       
        // `write your drag code here`
    }

    void do_movement(GLfloat delta)
    {
        // Camera controls
        if (keys[GLFW_KEY_UP])
        {
            camera.changePosZ(-delta);
        }
        else if (keys[GLFW_KEY_DOWN])
        {
            camera.changePosZ(delta);
        }

      
    }

    void ScreenPosToWorldRay(
        int lastX, int lastY,             // Mouse position, in pixels, from bottom-left corner of the window
        glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
        glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
    ) {

        // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
        glm::vec4 lRayStart_NDC(
            ((float)lastX / (float)width - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
            ((float)lastY / (float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f
        );

        glm::vec4 lRayEnd_NDC(
            ((float)lastX / (float)width - 0.5f) * 2.0f,
            ((float)lastY / (float)height - 0.5f) * 2.0f,
            0.0,
            1.0f
        );

        // The Projection matrix goes from Camera Space to NDC.
        // So inverse(ProjectionMatrix) goes from NDC to Camera Space.
        glm::mat4 InverseProjectionMatrix = glm::inverse(projection);

        // The View Matrix goes from World Space to Camera Space.
        // So inverse(ViewMatrix) goes from Camera Space to World Space.
        glm::mat4 InverseViewMatrix = glm::inverse(view);

        // Faster way (just one inverse)
        glm::mat4 M = glm::inverse(projection * view);
        glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; lRayEnd_world  /=lRayEnd_world.w;


        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);

        out_origin = glm::vec3(lRayStart_world);
        out_direction = glm::normalize(lRayDir_world);
    }

    void run()
    {
        // Build and compile our shader program
        Shader basic_shader("shaders/color_shader.vs", "shaders/color_shader.frag");
        Shader normals_shader("shaders/normals_shader.vs", "shaders/normals_shader.frag", "shaders/normals_shader.geom");
        
        // Get the uniform locations
        int modelLoc = glGetUniformLocation(basic_shader.Program, "model");
        int viewLoc = glGetUniformLocation(basic_shader.Program, "view");
        int projLoc = glGetUniformLocation(basic_shader.Program, "projection");
        int colorLoc = glGetUniformLocation(basic_shader.Program, "diffuseColor");

        int modelLoc_ns = glGetUniformLocation(normals_shader.Program, "model");
        int viewLoc_ns = glGetUniformLocation(normals_shader.Program, "view");
        int projLoc_ns = glGetUniformLocation(normals_shader.Program, "projection");
        int colorLoc_ns = glGetUniformLocation(normals_shader.Program, "normalsColor");
        int scaleLoc_ns = glGetUniformLocation(normals_shader.Program, "normalScale");
       
        glm::mat4 objModel, boxModel;
        glm::vec3 boxColor, normalsColor;

        while (!glfwWindowShouldClose(window))
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
    const GLuint width = 1600, height = 800;

    // Camera
    float xoffset = 0,  yoffset = 0; 
    GLfloat lastX = width / 2.0;
    GLfloat lastY = height / 2.0;
   
    // Deltatime
    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    GLfloat angle = 0.0f;

    bool firstMouse = true;
    bool lbutton_down = true;
    bool keys[1024];

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    glm::vec3 bgColor;
    glm::mat4 view, projection;

    // Limit values
    float max_float = std::numeric_limits<float>::max();
};