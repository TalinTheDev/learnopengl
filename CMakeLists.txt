cmake_minimum_required(VERSION 4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(learnopengl src/main.cpp)

add_library(learnopengllib STATIC src/glad.c)

target_include_directories(learnopengllib PUBLIC include)

file(GLOB IMGUI imgui/*.cpp imgui/*.h imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_opengl3.cpp imgui/backend/imgui_impl_opengl3.h)
set(IMGUI_SRC ${IMGUI})

add_library(imgui STATIC ${IMGUI_SRC})
target_include_directories(imgui PUBLIC imgui imgui/backends)

target_link_libraries(learnopengl PUBLIC imgui)
target_link_libraries(learnopengl PUBLIC learnopengllib)
target_link_libraries(learnopengl PUBLIC glfw)
