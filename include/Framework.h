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

        // Create a GLFWwindow object that we can use for GLFW's functions
        window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto 2 CG1 - UCV", nullptr, nullptr);
        glfwMakeContextCurrent(window);

        // Init Glad
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

        // OpenGL options
        glEnable(GL_DEPTH_TEST);

        setCallbacks();
        ui = UI(window);
        ui.init();

        // Create default material
        materials.push_back({ "Default", { 0.7, 0.7, 0.7 } });
        default_mtl = &materials[0];
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

    void mouse_callback(double xpos, double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        GLfloat xoffset = xpos - lastX;
        // Reversed since y-coordinates go from bottom to left
        GLfloat yoffset = lastY - ypos;
        //glfwSetCursorPos(window, WIDTH/2, HEIGHT/2);

        lastX = WIDTH / 2;// xpos;
        lastY = HEIGHT / 2; // ypos;
    }

    void do_movement(GLfloat delta)
    {
        // Camera controls
        if (keys[GLFW_KEY_W])
        {
            CameraPos.z += delta;
        }
        if (keys[GLFW_KEY_S])
        {
            CameraPos.z -= delta;
        }
        if (keys[GLFW_KEY_A])
        {
            CameraPos.x -= delta;
        }
        if (keys[GLFW_KEY_D])
        {
            CameraPos.x += delta;
        }
    }

    void run()
    {
        // Build and compile our shader program
        Shader basic_shader("shaders/color_shader.vs", "shaders/color_shader.frag");
        
        // Get the uniform locations
        //GLint objectColorLoc = glGetUniformLocation(basic_shader.Program, "objectColor");
        GLint modelLoc = glGetUniformLocation(basic_shader.Program, "model");
        GLint viewLoc = glGetUniformLocation(basic_shader.Program, "view");
        GLint projLoc = glGetUniformLocation(basic_shader.Program, "projection");
       
        GLuint renderedTexture;

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
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            static float x_angle = 0.0f;
            x_angle += deltaTime;


            if (draw)
            {
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


                glm::mat4 view = glm::lookAt(CameraPos, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                glm::mat4 projection = glm::perspective(60.0f * 3.14159f / 180.0f, float(w) / float(h), 0.1f, 100.0f);
                glm::mat4 model;

                // Use cooresponding shader when setting uniforms/drawing objects
                //glUniform3f(objectColorLoc, 1.0f, 1.0f, 1.0f);

                basic_shader.Use();

                // Pass the matrices to the shader
                glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

               

                for (int i = 0; i < models.size(); i++)
                {
                    for (int j = 0; j < models[i].meshes.size(); j++)
                    {
                        model = models[i].meshes[j].getModelTransformation();
                        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                        models[i].meshes[j].draw();
                    }
                }

                if (selectedMesh)
                {
                    model = selectedMesh->boundingBox.getModelTransformation();
                    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
                    selectedMesh->boundingBox.draw();
                }
            }
               
            // Render UI frame
            ui.render();
            // glPolygonOffset -- Z-Fighting, borde sobre el relleno, sumarle 1 bit al z
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
    const GLuint WIDTH = 1600, HEIGHT = 1600;

    // Camera
    GLfloat lastX = WIDTH / 2.0;
    GLfloat lastY = HEIGHT / 2.0;
    glm::vec3 CameraPos = glm::vec3(0.0, 0.0, 2.0);

    // Deltatime
    GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
    GLfloat lastFrame = 0.0f;  	// Time of last frame

    GLfloat angle = 0.0f;

    bool firstMouse = true;
    bool keys[1024];

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};