#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "fmap.h"

#define GLEW_STATIC

static void clean(void) {
  glfwTerminate();
}

static GLFWwindow* init(void) {
  GLFWwindow* window;

  if (glfwInit() == GLFW_FALSE) {
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  if((window = glfwCreateWindow(1000, 1000, "Window", NULL, NULL)) == NULL) {
    return NULL;
  }

  glfwMakeContextCurrent(window);

  if(glewInit() != GLEW_OK) {
    return NULL;
  }

  return window;
}

GLuint compileShader(const char* src, GLint type) {
  GLuint shader = glCreateShader(type);
  GLint status;

  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if(status == GL_FALSE) {
    char log[512];
    glGetShaderInfoLog(shader, sizeof(log), NULL, log);
    perror(log);
    return -1;
  }

  return shader;
}

void linkShaders(const int program, const int length, ...) {
  va_list shaders;

  va_start(shaders, length);

  for(int i = 0; i < length; i++) {
    glAttachShader(program, va_arg(shaders, GLuint));
  }

  va_end(shaders);
  glLinkProgram(program);
}

void shape(int row, int col, float vertices[row][col]) {
  float angle = (float) (M_PI / 2);
  for(int i = 0; i < row; i++) {
    vertices[i][0] = (float) (0.5 * cos(angle));
    vertices[i][1] = (float) (0.5 * sin(angle));
    angle += (float) ((2 * M_PI) / row);
  }
}

int main(void) {
  GLFWwindow* window;
  GLuint vao;
  GLuint vbo;
  GLuint ebo;
  GLuint program;
  GLuint vshader;
  GLuint fshader;
  GLuint elements[] = {
    0, 1, 2,
    2, 3, 0
  };
  GLint positionAttrib;
  GLint colorAttrib;
  char* vglsl = fmap("shaders/vertex.glsl");
  char* fglsl = fmap("shaders/fragment.glsl");
  float vertices[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  atexit(&clean);

  if((window = init()) == NULL) {
    return -1;
  }

  shape(4, 5, vertices);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  program = glCreateProgram();
  vshader = compileShader(vglsl, GL_VERTEX_SHADER);
  fshader = compileShader(fglsl, GL_FRAGMENT_SHADER);

  linkShaders(program, 2, vshader, fshader);
  glUseProgram(program);
  funmap(vglsl);
  funmap(fglsl);

  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

  positionAttrib = glGetAttribLocation(program, "position");
  glVertexAttribPointer(positionAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
  glEnableVertexAttribArray(positionAttrib);

  colorAttrib = glGetAttribLocation(program, "color");
  glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)));
  glEnableVertexAttribArray(colorAttrib);

  while(!glfwWindowShouldClose(window)) {
    usleep(100000);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, sizeof(elements), GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
