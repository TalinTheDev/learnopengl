// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

// Global Constants
const int WIDTH = 800;
const int HEIGHT = 600;

// Vertices for the triangle defined as the 3 corners and its colors
// const float vertices[] = {
//     // positions        // colors
//     0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
//     -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
//     0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
// };
// const unsigned int indices[] = {0, 1, 2};

// Vertices for the rectangle defined as the corners, their colors, and their
// texture coordinates
const float vertices[] = {
    // positions        // colors         // texture coordinates
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
};
const unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3, // second triangle
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

GLFWwindow *initalizeWindowContext() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window";
    glfwTerminate();
    return 0;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD";
    return 0;
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  return window;
};

unsigned int setupVAO() {
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  return VAO;
}

unsigned int setupVBO() {
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  return VBO;
}

unsigned int setupEBO() {
  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  return EBO;
}

void draw(Shader shader, unsigned int VAO, unsigned int texture1,
          unsigned int texture2) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, texture2);
  shader.use();
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}

unsigned int setupTexture(const char *path, GLenum type) {
  stbi_set_flip_vertically_on_load(true);
  unsigned int texture;
  glGenTextures(1, &texture);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, nrChannels;
  unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, type,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  return texture;
}

int main() {
  GLFWwindow *window = initalizeWindowContext();
  if (!window) {
    return -1;
  }

  Shader ourShader("../src/shader.vert", "../src/shader.frag");

  unsigned int VAO = setupVAO();
  unsigned int VBO = setupVBO();
  unsigned int EBO = setupEBO();
  unsigned int texture1 = setupTexture("../assets/container.jpg", GL_RGB);
  unsigned int texture2 = setupTexture("../assets/awesomeface.png", GL_RGBA);

  // For a wireframe drawing, set the mode to GL_LINE rather than GL_FILL
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Tell OpenGL which texture unit each shader sample belongs to
  ourShader.use();
  ourShader.setInt("texture1", 0);
  ourShader.setInt("texture2", 1);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    draw(ourShader, VAO, texture1, texture2);

    glfwPollEvents();
    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwTerminate();

  return 0;
}
