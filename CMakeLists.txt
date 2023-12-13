cmake_minimum_required(VERSION 3.16)

# Sets project name
project(pixel-grove LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# added for clion - might not need idk
# set(CMAKE_PREFIX_PATH /Users/spago/Qt/6.5.1/macos)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Sets C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specifies required Qt components
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS OpenGL)
find_package(Qt6 REQUIRED COMPONENTS OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)

# Specifies .cpp and .h files to be passed to the compiler
add_executable(${PROJECT_NAME}
      src/main.cpp

      src/realtime.cpp
      src/mainwindow.cpp
      src/settings.cpp
        src/utils/sceneparser.cpp

      src/mainwindow.h
      src/realtime.h
      src/settings.h
      src/utils/scenedata.h
        src/utils/sceneparser.h
      src/utils/shaderloader.h
      src/utils/aspectratiowidget/aspectratiowidget.hpp
        src/terrain/terrain.cpp
        src/terrain/terrain.h
        src/camera/camera.cpp
        src/camera/camera.h
        src/camera/camera.cpp
        src/flowergen/lily.cpp
        src/skybox/skybox_utils.h

)

# Specifies other files
qt_add_resources(${PROJECT_NAME} "Resources"
  PREFIX
  "/"
  FILES
  resources/images/back.jpg
  resources/images/bottom.jpg
  resources/images/front.jpg
  resources/images/left.jpg
  resources/images/right.jpg
  resources/images/top.jpg
  resources/textures/terrain-rock1.png
  resources/textures/terrain-sandgrass1.png
  resources/shaders/terrain.frag
  resources/shaders/terrain.vert
  resources/shaders/skybox.frag
  resources/shaders/skybox.vert
  resources/shaders/phong.frag
  resources/shaders/phong.vert
)

# GLEW: this creates its library and allows you to `#include "GL/glew.h"`
add_library(StaticGLEW STATIC glew/src/glew.c)
include_directories(${PROJECT_NAME} PRIVATE glew/include)

# Specifies libraries to be linked (Qt components, glew, etc)
target_link_libraries(${PROJECT_NAME} PRIVATE
  Qt::Core
  Qt::Widgets
  Qt::OpenGL
  Qt::OpenGLWidgets
  Qt::Gui
  StaticGLEW
)

# GLEW: this provides support for Windows (including 64-bit)
if (WIN32)
  add_compile_definitions(GLEW_STATIC)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    opengl32
    glu32
  )
endif()


# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()

# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()

# Set this flag to silence warnings on Windows
if (MSVC OR MSYS OR MINGW)
  set(CMAKE_CXX_FLAGS "-Wno-volatile")
endif()

# Set this flag to silence warnings on MacOS
if (APPLE)
  set(CMAKE_CXX_FLAGS "-Wno-deprecated-volatile")
endif()
