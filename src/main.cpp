// clang-format off
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "shader.hpp"
#include <iostream>

// Callback function to resize the OpenGL viewport when the GLFW window's
// dimensions are changed
// Used with: glfwSetFramebufferSizeCallback
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Function to process GLFW inputs
// Currently just checks for an ESCAPE key press and if detected, sets the
// window state to should close
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// Global Constants
const int WIDTH = 800;
const int HEIGHT = 600;

int main() {
  // Initialize GLFW with OpenGL 3.3 Core
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create a 800x600 GLFW window and make sure it was properly created
  GLFWwindow *window =
      glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window";
    glfwTerminate();
    return -1;
  }

  // Makes the OpenGL context in window the current context in this thread
  glfwMakeContextCurrent(window);

  // Make sure GLAD was properly initialized
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD";
    return -1;
  }

  // Set the OpenGL viewport to start at (0, 0) and span 800x600 pixels
  glViewport(0, 0, WIDTH, HEIGHT);

  // Set the callback to call when the GLFW window size changes
  // framebuffer_size_callback resizes the OpenGL viewport accordingly
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  Shader ourShader("../src/shader.vert", "../src/shader.frag");

  // Vertices for the triangle defined as the 3 corners and its colors
  float vertices[] = {
      // positions          // colors
      0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
      0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // top
  };

  // A VAO is a Vertex Array Object
  // VAOs are used to store OpenGL Vertex Attribute Calls
  // This allows you to only configure vertex attribute pointers once
  // Then whenever the object is being drawn, we can just bind the VAO
  // Instead of repeating code, just switch to the appropriate VAO for each draw
  unsigned int VAO;
  // Generates a Vertex Array and stores its id in VAO
  glGenVertexArrays(1, &VAO);
  // Binds the VAO to make all future VBO/EBO calls stored inside the VAO
  glBindVertexArray(VAO);

  // A VBO is a Vertex Buffer Object
  // VBOs store a large amount of vertices in the GPU
  // VBOs are useful because they allow us to send a large amount of data to the
  // GPU all at once instead of vertex by vertex
  // Memory transfer between the CPU and GPU is slow, so this is preferred
  unsigned int VBO;
  // Generate the buffer object and store its id in VBO
  glGenBuffers(1, &VBO);
  // Binds the VBO buffer object to the OpenGL Array Buffer
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  // Copies the vertices into the buffer stored as a OpenGL Array Buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // This tells OpenGL how to interpret vertex data
  // The data being configured is found in the VBO that is currently bound to
  // GL_ARRAY_BUFFER
  //
  // Using a VAO allows us to only make this function call once per
  // configuration and then switch VAOs whenever a different configuration is
  // required. Instead of making this complicated call again and again VAOs
  // store this call's data inside them (basically I think).
  //
  // The first argument is the vertex attribute we are configuring. This number
  // comes from the location declaration in the vertex shader
  // Because we pass in 0 and the vertex shader says aPos is in position 0
  // The data we configure here gets inputted into the vertex shader as aPos
  // By doing this whole setup, we pass in the vertices in the current VBO to
  // the shader
  //
  // The second argument is the size of the vertex attribute
  // 3 because the vertex is a vex3
  //
  // The third argument is the type of data in the VBO
  //
  // The fourth argument is whether or not we want the data to be normalized
  // (0/-1 - 1) Not relavent to use right now so leaving as false
  //
  // The fifth argument is the stride
  // Tells OpenGL the space between consecutive vertex arguments
  // Each argument is 3 floats & the array is tightly packed (0 space between
  // elements) So the stride is 3 times the size of a float
  //
  // The sixth argument is the offset of where the data begins in the buffer
  // The cast is required because the parameter type is a void*
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)0); // Positions
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                        (void *)(3 * sizeof(float))); // Colors
  // Enables the vertex attribute at location 0 (as specified in the vertex
  // shader and in the first argument of the glVertexAttribPointer function
  // call)
  glEnableVertexAttribArray(0); // Positions
  glEnableVertexAttribArray(1); // Colors

  // For a wireframe drawing, set the mode to GL_LINE rather than GL_FILL
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // While the GLFW window should not close
  while (!glfwWindowShouldClose(window)) {
    // Process window inputs
    processInput(window);

    // Set the buffer clear color to a dark blue-green
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // Clear the current framebuffer's buffer
    // Because GL_COLOR_BUFFER_BIT is set, the buffer will be cleared to the
    // color defined in the glClearColor function call
    glClear(GL_COLOR_BUFFER_BIT);

    // Sets the shader program that future drawing calls should use
    ourShader.use();

    // Binds a Vertex Array Object so that the draw call will use the vertex
    // attribute pointer stored in VAO
    glBindVertexArray(VAO);

    // Draws the vertices specified in the currently bound VBO in the order
    // specified in the currently bound EBO
    //
    // The first argument is the rendering mode
    // We want triangles
    //
    // The second argument is the number of vertices we want to draw
    //
    // The third argument is the type of values found in indices
    //
    // The fourth argument is the indices offset
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Makes GLFW check for window inputs
    glfwPollEvents();

    // Makes GLFW swap the buffers for the window from the front rendered one to
    // the back (next-frame) one
    // The 2 buffers prevent artifacts when only a part of the next frame is
    // ready to render. Using 2 buffers allows the next frame to only be
    // rendered once its buffer is fully ready.
    glfwSwapBuffers(window);
  }

  // De-allocate resources
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);

  // Destroys all windows, cursors, and frees all resources
  glfwTerminate();

  // Return success code
  return 0;
}
