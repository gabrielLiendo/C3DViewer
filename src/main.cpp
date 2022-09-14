#include <iostream>
#include <cmath>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <cstddef>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
#include <limits>
#include "tinyfiledialogs.h"

// GLM Mathematics
#define GLM_FORCE_RADIANS// force everything in radian
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Utils.h"
#include "Material.h"
#include "OBB.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "MtlLoader.h"
#include "ObjLoader.h"
#include "UI.h"
#include "Framework.h"

int main()
{
    MyGlWindow app = MyGlWindow();
    app.run();
    app.terminate();
}