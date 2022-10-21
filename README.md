# C3DViewer
C3DViewer is a basic 3D program to view 3D obj models, their meshes and materials, and compose scenes by modifying these properties. The scenes are saved using the program's own format on a plain text file. Lighting is made using the Phong illumination model, the respective light's properties can also be changed to ensure a better control upon the scene. Textures support is missing but it's currently being developt.

## Technologies
This project uses C++ as the main programming language, OpenGL as the graphics API, GLFW for window managing, TinyFileDialog to manage file dialogs, GLM for math operations and ImGui to present an amicable interface.

## Building
To build this program we use CMake, to do this we go to the main directory, the one that contains the CMakeLists.txt, and we execute the following commands: 