cmake_minimum_required(VERSION 3.0.0)
project(C3DViewer VERSION 0.1.0)

set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/lib/cmake/glfw3")

# Find package and libraries
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Create executable
file(GLOB IMGUI_SRC src/imgui/*.cpp)
set(SRCS 
    src/main.cpp 
    src/glad.c 
    src/tinyfiledialogs.c)
add_executable(${PROJECT_NAME} ${SRCS} ${IMGUI_SRC})

# Include
target_include_directories(${PROJECT_NAME} PUBLIC include/ include/imgui)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL glfw)