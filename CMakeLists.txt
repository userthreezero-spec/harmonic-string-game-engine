cmake_minimum_required(VERSION 3.20)
project(HarmonicStringEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(GLEW REQUIRED)

add_library(hse_core STATIC
    src/core/window.cpp
    src/renderer/renderer.cpp
    src/renderer/texture.cpp
    src/renderer/material.cpp
    src/scene/scene.cpp
    src/scene/camera.cpp
    src/scene/primitive.cpp
    src/scene/scene_builder.cpp
    src/math/vec3.cpp
    src/math/mat4.cpp
)

target_include_directories(hse_core PUBLIC include)
target_link_libraries(hse_core PUBLIC OpenGL::GL GLEW::GLEW glfw)

add_executable(hse_main src/main.cpp)
target_link_libraries(hse_main PRIVATE hse_core)

add_executable(hse_room_demo src/room_demo.cpp)
target_link_libraries(hse_room_demo PRIVATE hse_core)

add_executable(hse_test_math tests/test_math.cpp)
target_link_libraries(hse_test_math PRIVATE hse_core)

add_executable(hse_test_scene tests/test_scene.cpp)
target_link_libraries(hse_test_scene PRIVATE hse_core)

add_executable(hse_test_material tests/test_material.cpp)
target_link_libraries(hse_test_material PRIVATE hse_core)

add_executable(hse_test_hierarchy tests/test_hierarchy.cpp)
target_link_libraries(hse_test_hierarchy PRIVATE hse_core)

add_library(hse_bridge STATIC
    src/bridge/pipe_transport.cpp
    src/bridge/bridge.cpp
)
target_include_directories(hse_bridge PUBLIC include)
target_link_libraries(hse_bridge PUBLIC hse_core)

add_executable(hse_bridge_app src/bridge_main.cpp)
target_link_libraries(hse_bridge_app PRIVATE hse_bridge)

add_executable(hse_workspace_app src/workspace_main.cpp)
target_link_libraries(hse_workspace_app PRIVATE hse_bridge)

add_executable(hse_texture_demo src/texture_demo.cpp)
target_link_libraries(hse_texture_demo PRIVATE hse_core)

add_executable(hse_material_demo src/material_demo.cpp)
target_link_libraries(hse_material_demo PRIVATE hse_core)

enable_testing()
add_test(NAME MathTests COMMAND hse_test_math)
add_test(NAME SceneTests COMMAND hse_test_scene)
add_test(NAME MaterialTests COMMAND hse_test_material)
add_test(NAME HierarchyTests COMMAND hse_test_hierarchy)
