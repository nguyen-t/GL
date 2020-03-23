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

int main(void) {
  GLFWwindow* window;
  GLuint vao;
  GLuint vbo;
  GLuint program;
  GLuint vshader;
  GLuint fshader;
  GLint uniColor;
  GLint posAttrib;
  char* vglsl = fmap("shaders/vertex.glsl");
  char* fglsl = fmap("shaders/fragment.glsl");
  float vertices[] = {
    -0.5f, -0.5f,
     0.0f,  0.5f,
     0.5f, -0.5f
  };

  atexit(&clean);

  if((window = init()) == NULL) {
    return -1;
  }

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

  posAttrib = glGetAttribLocation(program, "position");
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(posAttrib);

  uniColor = glGetUniformLocation(program, "triangleColor");

  for(float i = 0; !glfwWindowShouldClose(window); i += .05) {
    usleep(100000);
    glClear(GL_COLOR_BUFFER_BIT);
    glUniform3f(uniColor, sin(i), cos(i), tan(i));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  return 0;
}
