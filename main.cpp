#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>


// -- Shaders ----
//Vertex Shaders: runs once per vertex ( corner of the triangle )
// gl_position is the final position on the screen
const char* vertexShaderSrc = R"(
 #version 440 core
 layout(location = 0) in vec3 aPos;

uniform vec2 offset; //receives position from C++

 void main() {
 gl_Position = vec4(aPos.x + offset.x, aPos.y + offset.y, aPos.z, 1.0);
 }
)";

const char* fragShaderSrc = R"(
#version 440 core
out vec4 outColor;
void main() {
    outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
)";

GLuint CompileShader (GLenum type, const char* src) {
 GLuint shader = glCreateShader(type);
 glShaderSource(shader, 1, &src, nullptr);
 glCompileShader(shader);
 int success;
 glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
 if (!success) {
  char infoLog[512];
  glGetShaderInfoLog(shader, 512, nullptr, infoLog);
  std::cerr << "Shader error:" << infoLog << "\n";
 }
 return shader;
}

int main() {
 SDL_Init(SDL_INIT_EVERYTHING);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

 SDL_Window* window = SDL_CreateWindow(
  "MY GAME", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
 SDL_GLContext glContext = SDL_GL_CreateContext(window);
 glewExperimental = GL_TRUE;
 glewInit();


 // --- Triangle vertices (X,Y,Z) in NDC Space
 // NDC = Normalized Device Coordinates
 // Center of screen = (0,0), corners = (-1 or 1)

 float vertices[] = {
  -0.5f, -0.5f, 0.0f,
   0.5f, -0.5f, 0.0f,
   0.5f,  0.5f, 0.0f,
  -0.5f, -0.5f, 0.0f,
   0.5f,  0.5f, 0.0f,
  -0.5f,  0.5f, 0.0f
};

 // 1. VAO first
 GLuint VAO;
 glGenVertexArrays(1, &VAO);
 glBindVertexArray(VAO);

 // 2. VBO second
 GLuint VBO;
 glGenBuffers(1, &VBO);
 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

 // 3. Tell VAO how to read the VBO
 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(0);

 GLuint vertShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc);
 GLuint fragShader = CompileShader(GL_FRAGMENT_SHADER, fragShaderSrc);

 GLuint shaderProgram = glCreateProgram();
 glAttachShader(shaderProgram, vertShader);
 glAttachShader(shaderProgram, fragShader);
 glLinkProgram(shaderProgram);

 //Shaders are compiled into the program, no longer needed separately
 glDeleteShader(vertShader);
 glDeleteShader(fragShader);

 float posX = 0.0f;
 float posY = 0.0f;
 float speed = 0.01f;

 // GAME LOOP
 bool running = true;
 SDL_Event event;

 while (running) {
  while (SDL_PollEvent(&event)) {
   if (event.type == SDL_QUIT) running = false;
   if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
  }

  //check keyboard state every frame
  const Uint8* keys = SDL_GetKeyboardState(NULL);
  if (keys[SDL_SCANCODE_W]) posY += speed;
  if (keys[SDL_SCANCODE_S]) posY -= speed;
  if (keys[SDL_SCANCODE_A]) posX -= speed;
  if (keys[SDL_SCANCODE_D]) posX += speed;

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  glUseProgram(shaderProgram);

  GLint offsetLocation = glGetUniformLocation(shaderProgram, "offset");
  glUniform2f(offsetLocation, posX, posY);
  glBindVertexArray(VAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  SDL_GL_SwapWindow(window);
 }


 // cleanup
 glDeleteVertexArrays(1, &VAO);
 glDeleteBuffers(1, &VBO);
 glDeleteProgram(shaderProgram);
 SDL_GL_DeleteContext(glContext);
 SDL_DestroyWindow(window);
 SDL_Quit();
 return 0;
}
