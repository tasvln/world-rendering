#include <SDL.h>
#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "src/camera/orbit.h"
#include "src/camera/fps.h"

using namespace std;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 768;

float pVertices[] = {
    -1000.0f, 0.0f, -1000.0f,
    1000.0f, 0.0f, -1000.0f,
    1000.0f, 0.0f, 1000.0f,
    -1000.0f, 0.0f, 1000.0f};

unsigned int pIndices[] = {
    0, 1, 2,
    2, 3, 0};

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_GLContext context;

// Open GL vars
GLuint gridShaderProgram, originDotShaderProgram;
GLuint gridVAO, gridVBO, gridEBO;
GLuint dotVAO, dotVBO;

OrbitCamera orbitCam(
    glm::vec3(0.0f), // Target is origin
    15.0f,           // Radius distance from target
    -45.0f,          // Yaw
    20.0f            // Pitch
);

FPSCamera fpsCam(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), -90.0f, 0.0f);

void close();