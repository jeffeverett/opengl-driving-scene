cmake_minimum_required(VERSION 3.0)
project(final)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

# Find GLFW3
find_package(glfw3 3.2 REQUIRED)
include_directories(${glfw3_INCLUDE_DIRS})
link_directories(${glfw3_LIBRARY_DIRS})
add_definitions(${glfw3_DEFINITIONS})

if(glfw3_FOUND)
    message("glfw3 found")
else(glfw3_FOUND)
    message(FATAL_ERROR "glfw3 not found")
endif(glfw3_FOUND)

# Find GLM
find_package(glm REQUIRED)
include_directories(${glm_INCLUDE_DIRS})
link_directories(${glm_LIBRARY_DIRS})
add_definitions(${glm_DEFINITIONS})

if(glm_FOUND)
    message("glm found")
else(glm_FOUND)
    message(FATAL_ERROR "glm not found")
endif(glm_FOUND)

# Find Bullet
find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
link_directories(${BULLET_LIBRARY_DIRS})
add_definitions(${BULLET_DEFINITIONS})

if(BULLET_FOUND)
    message("Bullet found")
else(BULLET_FOUND)
    message(FATAL_ERROR "Bullet not found")
endif(BULLET_FOUND)

# Find assimp
find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIRS})
link_directories(${assimp_LIBRARY_DIRS})

if(ASSIMP_FOUND)
    message("assimp found")
else(ASSIMP_FOUND)
    message(FATAL_ERROR "assimp not found")
endif(ASSIMP_FOUND)


# Find freetype
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
link_directories(${FREETYPE_LIBRARY_DIRS})
add_definitions(${FREETYPE_DEFINITIONS})

if(FREETYPE_FOUND)
    message("Freetype found")
else(FREETYPE_FOUND)
    message(FATAL_ERROR "Freetype not found")
endif(FREETYPE_FOUND)

# Find OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL 4.4 REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

if(OPENGL_FOUND)
    message("OpenGL found")
else(OPENGL_FOUND)
    message(FATAL_ERROR "OpenGL not found")
endif(OPENGL_FOUND)

include_directories(Code/Headers/)
include_directories(SYSTEM Code/Vendor/glad/include/ Code/Vendor/stb/)


file(GLOB VENDORS_SOURCES Code/Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS
    Code/Headers/*.hpp Code/Headers/Assets/*.hpp Code/Headers/Components/*.hpp
    Code/Headers/Core/*.hpp Code/Headers/Objects/*.hpp Code/Headers/Physics/*.hpp
    Code/Headers/Rendering/*.hpp Code/Headers/Scripts/*.hpp Code/Headers/Utils/*.hpp)
file(GLOB PROJECT_SOURCES
    Code/Sources/*.cpp Code/Sources/Assets/*.cpp Code/Sources/Components/*.cpp
    Code/Sources/Core/*.cpp Code/Sources/Objects/*.cpp Code/Sources/Physics/*.cpp
    Code/Sources/Rendering/*.cpp Code/Sources/Scripts/*.cpp Code/Sources/Utils/*.cpp)
file(GLOB PROJECT_SHADERS
        Code/Shaders/*.comp
        Code/Shaders/*.frag
        Code/Shaders/*.geom
        Code/Shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARIES} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath freetype)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
