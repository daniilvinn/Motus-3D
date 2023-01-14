cmake_minimum_required(VERSION 3.10)
set (CMAKE_SYSTEM_VERSION 10.0.22621.0)
project(Motus3D)

include("dependencies.cmake")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/obj/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

add_subdirectory(Motus3D)
add_subdirectory(Visus)
add_subdirectory(Sandbox2D)
add_subdirectory(Scene)
add_subdirectory("Motus3D/thirdparty/GLFW")
add_subdirectory("Motus3D/thirdparty/Assimp")
add_subdirectory("Motus3D/thirdparty/imgui")

set(CMAKE_DEFAULT_STARTUP_PROJECT Sandbox)