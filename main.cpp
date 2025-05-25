#include "main.h"

// Helper Functions

void close()
{
  if (worldModel)
    delete worldModel;
  if (gridEBO)
    glDeleteBuffers(1, &gridEBO);
  if (gridVBO)
    glDeleteBuffers(1, &gridVBO);
  if (gridVAO)
    glDeleteVertexArrays(1, &gridVAO);
  if (dotVBO)
    glDeleteBuffers(1, &dotVBO);
  if (dotVAO)
    glDeleteVertexArrays(1, &dotVAO);
  if (gridShaderProgram)
    glDeleteProgram(gridShaderProgram);
  if (originDotShaderProgram)
    glDeleteProgram(originDotShaderProgram);
  if (context)
    SDL_GL_DestroyContext(context);
  if (window)
    SDL_DestroyWindow(window);
  SDL_Quit();
}

void checkShaderCompilation(GLuint shader, const char *type)
{
  GLint success;
  GLchar infoLog[1024];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    std::cerr << "Shader Compilation Error (" << type << "): " << infoLog << std::endl;
  }
}

void checkShaderLinking(GLuint program)
{
  GLint success;
  GLchar infoLog[1024];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success)
  {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    std::cerr << "Shader Linking Error: " << infoLog << std::endl;
  }
}

// Main Functions

bool init()
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  // Request OpenGL 3.3 Core Profile (Required on macOS)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  // Create Window
  window = SDL_CreateWindow("grid plane", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (!window)
  {
    cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
    close();
    return false;
  }

  // Create GL context
  context = SDL_GL_CreateContext(window);
  if (!context)
  {
    cerr << "SDL_GL_CreateContext Error: " << SDL_GetError() << endl;
    close();
    return false;
  }

  // Fixing driver Issues
  glewExperimental = GL_TRUE;

  GLenum glErr = glewInit();

  if (glErr != GLEW_OK)
  {
    cerr << "GLEW Init Error: " << glewGetErrorString(glErr) << endl;
    close();
    return false;
  }

  if (!SDL_GL_SetSwapInterval(1))
  {
    cerr << "Unable to set VSync! SDL Error: " << SDL_GetError() << endl;
    return false;
  }

  return true;
}

GLuint loadShader(const char *vertexPath, const char *fragPath)
{
  GLuint shaderProgram = glCreateProgram();

  ifstream vertexFile(vertexPath), fragFile(fragPath);
  stringstream vertexStream, fragStream;

  vertexStream << vertexFile.rdbuf();
  fragStream << fragFile.rdbuf();

  string vertexCode = vertexStream.str();
  string fragCode = fragStream.str();

  const char *vertexStr = vertexCode.c_str();
  const char *fragStr = fragCode.c_str();

  // vertex creation
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexStr, NULL);
  glCompileShader(vertexShader);
  checkShaderCompilation(vertexShader, "vertex");

  // fragment creation
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragStr, NULL);
  glCompileShader(fragShader);
  checkShaderCompilation(fragShader, "fragment");

  // shader binding
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragShader);
  glLinkProgram(shaderProgram);
  checkShaderLinking(shaderProgram);

  // delete shaders after linking
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  return shaderProgram;
}

bool initGL()
{
  gridShaderProgram = loadShader("src/shaders/infiniteGrid/vertex.glsl", "src/shaders/infiniteGrid/frag.glsl");

  glGenVertexArrays(1, &gridVAO);
  glGenBuffers(1, &gridVBO);
  glGenBuffers(1, &gridEBO);

  glBindVertexArray(gridVAO);

  // vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pVertices), pVertices, GL_STATIC_DRAW);

  // element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gridEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pIndices), pIndices, GL_STATIC_DRAW);

  // TO DO: fully understand how this works
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  return true;
}

bool drawOriginDot()
{
  originDotShaderProgram = loadShader("src/shaders/centerAnchor/vertex.glsl", "src/shaders/centerAnchor/frag.glsl");

  float dotVertices[] = {
      0.0f, 0.0f, 0.0f};

  glGenVertexArrays(1, &dotVAO);
  glGenBuffers(1, &dotVBO);

  glBindVertexArray(dotVAO);

  glBindBuffer(GL_ARRAY_BUFFER, dotVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(dotVertices), dotVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return true;
}

bool drawWorldModel()
{
  worldProgram = loadShader("src/shaders/worldModel/vertex.glsl", "src/shaders/worldModel/frag.glsl");
  worldModel = new nsi::World("ext/models/mountain1/mesh_range01_05K_OBJ.obj", glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));

  return true;
}

void update()
{
  // FPS Update logic
}

void handleOrbitMouseMovement(SDL_Event event)
{
  if (event.type == SDL_EVENT_MOUSE_MOTION)
  {
    // relative motion
    float xoffset = event.motion.xrel;
    // invert y for Blender-like feel
    float yoffset = -event.motion.yrel;

    orbitCam.processMouseMovement(xoffset, yoffset);
  }
}

void handleOrbitZoom(SDL_Event event)
{
  if (event.type == SDL_EVENT_MOUSE_WHEEL)
  {
    orbitCam.processMouseScroll(static_cast<float>(event.wheel.y));
  }
}

void handleFPSMouseMovement(SDL_Event event)
{
  if (event.type == SDL_EVENT_MOUSE_MOTION)
  {
    float xoffset = event.motion.xrel;
    float yoffset = -event.motion.yrel; // Invert y for typical FPS
    fpsCam.processMouseMovement(xoffset, yoffset);
  }
}

void handleFPSKeyMovement(SDL_Scancode key, float deltaTime)
{
  // GLuint num;
  const bool *keystates = SDL_GetKeyboardState(NULL);

  if (keystates[SDL_SCANCODE_W])
    fpsCam.processKeyboard("FORWARD", deltaTime);
  if (keystates[SDL_SCANCODE_S])
    fpsCam.processKeyboard("BACKWARD", deltaTime);
  if (keystates[SDL_SCANCODE_A])
    fpsCam.processKeyboard("LEFT", deltaTime);
  if (keystates[SDL_SCANCODE_D])
    fpsCam.processKeyboard("RIGHT", deltaTime);
  if (keystates[SDL_SCANCODE_SPACE])
    fpsCam.jump();
}

void handleKeys(SDL_Scancode key, float deltaTime)
{
  handleFPSKeyMovement(key, deltaTime);
}

void render()
{
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 model = worldModel->getModelMatrix();
  glm::mat4 view = orbitCam.getViewMatrix();
  // glm::mat4 view = fpsCam.getViewMatrix();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

  // Draw Grid
  glUseProgram(gridShaderProgram);
  GLuint modelLoc = glGetUniformLocation(gridShaderProgram, "model");
  GLuint viewLoc = glGetUniformLocation(gridShaderProgram, "view");
  GLuint projLoc = glGetUniformLocation(gridShaderProgram, "projection");

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
  glUniform1f(glGetUniformLocation(gridShaderProgram, "spacing"), 10.0f);

  glBindVertexArray(gridVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  glUseProgram(worldProgram);
  modelLoc = glGetUniformLocation(worldProgram, "model");
  viewLoc = glGetUniformLocation(worldProgram, "view");
  projLoc = glGetUniformLocation(worldProgram, "projection");

  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, worldModel->textureID);
  glUniform1i(glGetUniformLocation(worldProgram, "texture_diffuse1"), 0);

  worldModel->draw(worldProgram);
}

int main(int argc, char *argv[])
{
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  SDL_SetHint(SDL_HINT_TRACKPAD_IS_TOUCH_ONLY, "1");

  // SDL_SetWindowRelativeMouseMode(window, toggle ? SDL_TRUE : SDL_FALSE); -> for cam toggle implementation / keep relative true?
  SDL_SetWindowRelativeMouseMode(window, true);
  SDL_PumpEvents();

  if (!init())
  {
    cerr << "Failed to initialize" << endl;
    return -1;
  }

  if (!initGL())
  {
    cerr << "Failed to initialize OpenGl" << endl;
    return -1;
  }

  if (!drawOriginDot())
  {
    cerr << "Failed to initialize OriginDot" << endl;
    return -1;
  }

  if (!drawWorldModel())
  {
    cerr << "Failed to initialize WorldModel" << endl;
    return -1;
  }

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  SDL_Event evt;
  bool running = true;

  int lastX = SCREEN_WIDTH / 2, lastY = SCREEN_HEIGHT / 2;
  bool firstMouse = true;

  while (running)
  {
    float currentFrame = SDL_GetTicks() / 1000.0f;
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    while (SDL_PollEvent(&evt))
    {
      if (evt.type == SDL_EVENT_QUIT)
      {
        running = false;
      }
      if (evt.type == SDL_EVENT_KEY_DOWN)
      {
        handleKeys(evt.key.scancode, deltaTime);
        if (evt.key.scancode == SDLK_ESCAPE)
        {
          running = false;
        }
      }
      handleOrbitMouseMovement(evt);
      handleOrbitZoom(evt);
      // handleFPSMouseMovement(evt);
    }

    fpsCam.updatePhysics(deltaTime);

    render();

    SDL_GL_SwapWindow(window);
  }

  close();

  return 0;
}