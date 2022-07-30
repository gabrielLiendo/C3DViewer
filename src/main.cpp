#include <iostream>
#include <cmath>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <string>

#include "tinyfiledialogs.h"

// GLM Mathematics
#define GLM_FORCE_RADIANS// force everything in radian
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "3DObject.h"
#include "UI.h"
#include "Framework.h"


using namespace std;

int main()
{
    MyGlWindow app = MyGlWindow();
    app.run();
    app.terminate();
}