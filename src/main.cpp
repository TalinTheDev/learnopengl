// clang-format off
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "constants.cpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "utils/camera.hpp"
#include "utils/shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

// Global Variables
bool debugWindow = false;
float debugWindowShowTime = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void handleMovement(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS &&
      debugWindowShowTime > 0.1f) {
    debugWindow = !debugWindow;
    debugWindowShowTime = 0.0f;
  } else {
    debugWindowShowTime += 0.01f;
  }

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.move(Direction::Forward, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.move(Direction::Backward, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.move(Direction::Left, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.move(Direction::Right, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    camera.move(Direction::Up, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
    camera.move(Direction::Down, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    camera.look(camera.lastX, camera.lastY - 10);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    camera.look(camera.lastX, camera.lastY + 10);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    camera.look(camera.lastX - 10, camera.lastY);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    camera.look(camera.lastX + 10, camera.lastY);
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  camera.look(xpos, ypos);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.zoom(yoffset);
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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD";
    return 0;
  }

  glViewport(0, 0, WIDTH, HEIGHT);

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  return window;
};

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

  Shader cubeShader("../src/shaders/cube.vert", "../src/shaders/cube.frag");
  Shader lightShader("../src/shaders/light.vert", "../src/shaders/light.frag");

  unsigned int cubeVAO;
  unsigned int lightVAO;
  unsigned int VBO;

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glGenVertexArrays(1, &lightVAO);
  glBindVertexArray(lightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install GLFW
                     // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();

  // For a wireframe drawing, set the mode to GL_LINE rather than GL_FILL
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glEnable(GL_DEPTH_TEST);
  glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
  glm::vec3 lightColor(1.0f);
  float lightDiffuseIntensity = 0.5f;
  float lightAmbientIntensity = 0.2f;
  float lightSpecularIntensity = 1.0f;
  glm::vec3 cubeAmbientColor(1.0f, 0.5f, 0.31f);
  glm::vec3 cubeDiffuseColor(1.0f, 0.5f, 0.31f);
  glm::vec3 cubeSpecularColor(0.5f);
  float cubeShininess = 32.0f;
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    handleMovement(window);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view;
    view = glm::lookAt(camera.position, camera.position + camera.frontFace,
                       camera.upVec);
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(camera.fov), WIDTH / HEIGHT,
                                  0.1f, 100.0f);

    glm::vec3 lightDiffuseColor = lightColor * lightDiffuseIntensity;
    glm::vec3 lightAmbientColor = lightDiffuseColor * lightAmbientIntensity;
    glBindVertexArray(cubeVAO);
    glm::mat4 model = glm::mat4(1.0f);
    cubeShader.use();
    cubeShader.setVec3("viewPos", camera.position);
    cubeShader.setMat4("view", view);
    cubeShader.setMat4("projection", projection);
    cubeShader.setMat4("model", model);
    cubeShader.setVec3("material.ambient", cubeAmbientColor);
    cubeShader.setVec3("material.diffuse", cubeDiffuseColor);
    cubeShader.setVec3("material.specular", cubeSpecularColor);
    cubeShader.setFloat("material.shininess", cubeShininess);
    cubeShader.setVec3("light.position", lightPos);
    cubeShader.setVec3("light.ambient", lightAmbientColor);
    cubeShader.setVec3("light.diffuse", lightDiffuseColor);
    cubeShader.setVec3("light.specular", glm::vec3(lightSpecularIntensity));

    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

    glBindVertexArray(lightVAO);
    lightShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f));

    lightShader.setMat4("view", view);
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("model", model);
    lightShader.setVec3("light.color", lightColor);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

    if (debugWindow) {
      ImGui::SetNextWindowSize(ImVec2(WIDTH / 3, HEIGHT));
      ImGui::SetNextWindowPos(ImVec2(0, 0));
      ImGui::Begin("LearnOpenGL Debug Window");
      if (ImGui::CollapsingHeader("Camera")) {
        if (ImGui::Button("Reset")) {
          camera.fov = 45.0f;
          camera.position = glm::vec3(0.0f, 2.0f, 4.0f);
          camera.frontFace = glm::vec3(0.0f, 0.0f, 0.0f);
          camera.yaw = -77.0f;
          camera.pitch = -25.0f;
          camera.moveSpeed = 2.5f;
          camera.lookSens = 0.1f;
          camera.look(camera.lastX, camera.lastY);
        }
        ImGui::SliderFloat3("Camera Position", (float *)&camera.position, -5.0f,
                            5.0f);
        ImGui::SliderFloat("Camera Yaw", &camera.yaw, -360.0f, 360.0f);
        ImGui::SliderFloat("Camera Pitch", &camera.pitch, -89.0f, 89.0f);
        ImGui::SliderFloat("Camera FOV", &camera.fov, 1.0f, 120.0f);
        ImGui::SliderFloat("Movement Speed", &camera.moveSpeed, 0.0f, 10.0f);
        ImGui::SliderFloat("Look Speed", &camera.lookSens, 0.0f, 10.0f);
      }
      if (ImGui::CollapsingHeader("Lighting")) {
        if (ImGui::Button("Reset Lighting")) {
          lightColor = glm::vec3(1.0f);
          lightDiffuseIntensity = 0.5f;
          lightAmbientIntensity = 0.2f;
          lightSpecularIntensity = 1.0f;
          cubeAmbientColor = glm::vec3(1.0f, 0.5f, 0.31f);
          cubeDiffuseColor = glm::vec3(1.0f, 0.5f, 0.31f);
          cubeSpecularColor = glm::vec3(0.5f);
          cubeShininess = 32.0f;
        }
        ImGui::ColorEdit3("Light Color", (float *)&lightColor);
        ImGui::SliderFloat("Light Ambient Intensity", &lightAmbientIntensity, 0,
                           1);
        ImGui::SliderFloat("Light Diffuse Intensity", &lightDiffuseIntensity, 0,
                           1);
        ImGui::SliderFloat("Light Specular Intensity", &lightSpecularIntensity,
                           0, 1);

        ImGui::SliderFloat3("Cube Ambient Color", (float *)&cubeAmbientColor, 0,
                            1);
        ImGui::SliderFloat3("Cube Diffuse Color", (float *)&cubeDiffuseColor, 0,
                            1);
        ImGui::SliderFloat3("Cube Specular Color", (float *)&cubeSpecularColor,
                            0, 1);
        ImGui::SliderFloat("Cube Shininess", &cubeShininess, 0, 512);
      }

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / io.Framerate, io.Framerate);
      ImGui::End();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &VBO);
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();

  return 0;
}
