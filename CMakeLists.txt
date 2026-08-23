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
    src/scene/picker.cpp
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

add_executable(hse_test_orbit tests/test_orbit.cpp)
target_link_libraries(hse_test_orbit PRIVATE hse_core)

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

add_executable(hse_orbit_demo src/orbit_demo.cpp)
target_link_libraries(hse_orbit_demo PRIVATE hse_core)

enable_testing()
add_test(NAME MathTests COMMAND hse_test_math)
add_test(NAME SceneTests COMMAND hse_test_scene)
add_test(NAME MaterialTests COMMAND hse_test_material)
add_test(NAME HierarchyTests COMMAND hse_test_hierarchy)
add_test(NAME OrbitTests COMMAND hse_test_orbit)

add_executable(hse_test_hsc tests/test_hsc_persistence.cpp)
target_link_libraries(hse_test_hsc PRIVATE hse_core)
add_test(NAME PersistenceTests COMMAND hse_test_hsc)

add_executable(hse_test_input tests/test_input.cpp)
target_link_libraries(hse_test_input PRIVATE hse_core)
add_test(NAME InputTests COMMAND hse_test_input)

add_executable(hse_test_picking tests/test_picking.cpp)
target_link_libraries(hse_test_picking PRIVATE hse_core)
add_test(NAME PickingTests COMMAND hse_test_picking)

add_executable(hse_test_collision tests/test_collision.cpp)
target_link_libraries(hse_test_collision PRIVATE hse_core)
add_test(NAME CollisionTests COMMAND hse_test_collision)

add_executable(hse_test_assembly tests/test_assembly.cpp)
target_link_libraries(hse_test_assembly PRIVATE hse_core)
add_test(NAME AssemblyTests COMMAND hse_test_assembly)

add_executable(hse_test_assembly_persistence tests/test_assembly_persistence.cpp)
target_link_libraries(hse_test_assembly_persistence PRIVATE hse_core)
add_test(NAME AssemblyPersistenceTests COMMAND hse_test_assembly_persistence)

add_executable(hse_test_goal tests/test_goal_conformance.cpp)
target_link_libraries(hse_test_goal PRIVATE hse_core)
add_test(NAME GoalTests COMMAND hse_test_goal)

add_executable(hse_test_multi tests/test_multi_constraint.cpp)
target_link_libraries(hse_test_multi PRIVATE hse_core)
add_test(NAME MultiConstraintTests COMMAND hse_test_multi)

# [WebOS Native Engineering Actuator] runtime DLL deployment
# Deploys the toolchain/runtime DLLs next to the executable so the built product
# launches without the toolchain on PATH (fixes STATUS_DLL_NOT_FOUND / 0xC0000135).
if(WIN32)
    get_filename_component(_webos_tc_bin "${CMAKE_CXX_COMPILER}" DIRECTORY)
    set(_webos_runtime_dlls
        libgcc_s_seh-1.dll
        libstdc++-6.dll
        glew32.dll
        glfw3.dll
        libwinpthread-1.dll)
    foreach(_dll ${_webos_runtime_dlls})
        if(EXISTS "${_webos_tc_bin}/${_dll}")
            add_custom_command(TARGET hse_main POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_webos_tc_bin}/${_dll}" "$<TARGET_FILE_DIR:hse_main>")
        endif()
    endforeach()
endif()
