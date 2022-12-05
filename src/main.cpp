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

// Image loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// GLM Mathematics
#define GLM_FORCE_RADIANS// force everything in radian
#define PI 3.141592654
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Other includes
#include "Texture.h"
#include "Shader.h"
#include "Utils.h"
#include "Material.h"
#include "OBB.h"
#include "Mesh.h"
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include "Scene.h"
#include "MtlLoader.h"
#include "ObjLoader.h"
#include "LoadersManager.h"
#include "UI.h"
#include "C3DViewer.h"

int main()
{
    std::unique_ptr<C3DViewer> app = std::make_unique<C3DViewer>();
    app->run();
}