# C3DViewer
C3DViewer is a basic 3D program to view obj models, their meshes and materials, and compose scenes by modifying these properties. The scenes are saved using the program's own format on a plain text file. Lighting is made using the Phong illumination model, the respective light's properties can also be changed to ensure a better control upon the scene. Textures support is missing but it's currently in development.


![Screenshot 2022-10-21 000803](https://user-images.githubusercontent.com/35168412/197109541-9a83c577-7d9f-44eb-b16e-3309ce317204.png)


## Technologies
This project uses C++ as the main programming language, OpenGL as the graphics API, GLFW for window managing, TinyFileDialog to manage file dialogs, GLM for math operations and ImGui to present an amicable interface.

## Building
To build this program we use CMake, to do this we go to the main directory, the one that contains the CMakeLists.txt, and we execute the following commands: 

```
cmake -S . -B ./build -G Ninja

cmake --build ./build --config Debug --target C3DViewer

./build/C3DViewer
```