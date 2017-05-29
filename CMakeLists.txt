cmake_minimum_required(VERSION 3.7)
project(RayTracer)

set(CMAKE_CXX_STANDARD 14)

#########################################################
# FIND GLUT
#########################################################
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

#########################################################
# FIND OPENGL
#########################################################
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
endif(NOT OPENGL_FOUND)

add_definitions("-msse3")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")

set(SOURCE_FILES
        RayTracer/AlignedObject.cpp
        RayTracer/AlignedObject.h
        RayTracer/Cone.cpp
        RayTracer/Cone.h
        RayTracer/Cylinder.cpp
        RayTracer/Cylinder.h
        RayTracer/Image.cpp
        RayTracer/Image.h
        RayTracer/InfinitePlane.cpp
        RayTracer/InfinitePlane.h
        RayTracer/JsonSceneLoader.cpp
        RayTracer/JsonSceneLoader.h
        RayTracer/Light.h
        RayTracer/main.cpp
        RayTracer/Material.cpp
        RayTracer/Material.h
        RayTracer/MathsHelper.h
        RayTracer/Polygon.cpp
        RayTracer/Polygon.h
        RayTracer/Ray.h
        RayTracer/RayTracer.cpp
        RayTracer/RayTracer.h
        RayTracer/SceneObject.h
        RayTracer/Sphere.cpp
        RayTracer/Sphere.h
        RayTracer/Torus.cpp
        RayTracer/Torus.h
        RayTracer/vec4.h)

add_executable(RayTracer ${SOURCE_FILES})
target_link_libraries(RayTracer ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} SOIL)

file(GLOB SCENE_FILES "${CMAKE_SOURCE_DIR}/RayTracer/*.json")
add_custom_command(TARGET RayTracer POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SCENE_FILES} $<TARGET_FILE_DIR:RayTracer>)