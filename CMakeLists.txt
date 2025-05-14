cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(WINDOW VERSION 1.0.0)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Variables storing SDL framework locations
set(SDL
  /Library/Frameworks/SDL3.framework)
set(SDL_image
  /Library/Frameworks/SDL3_image.framework)
set(SDL_ttf
  /Library/Frameworks/SDL3_ttf.framework)

add_executable(WINDOW main.cpp)

include_directories(
  ${CMAKE_SOURCE_DIR}/src/include
)

target_link_libraries(
  WINDOW PRIVATE
  ${SDL}/Versions/A/SDL3
  ${SDL_image}/Versions/A/SDL3_image
  ${SDL_ttf}/Versions/A/SDL3_ttf
  OpenGL::GL
  GLEW::GLEW
)

target_include_directories(
  WINDOW PRIVATE
  ${SDL}/Versions/A/Headers
  ${SDL_image}/Versions/A/Headers
  ${SDL_ttf}/Versions/A/Headers
)