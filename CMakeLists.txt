cmake_minimum_required(VERSION 4.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(learnopengl)

add_executable(learnopengl src/main.cpp)

add_library(learnopengllib STATIC src/glad.c)
target_include_directories(learnopengllib PUBLIC include)
target_link_libraries(learnopengl PUBLIC learnopengllib)
target_link_libraries(learnopengl PUBLIC glfw)
