// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cstdio>

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

// Basic vertex shader source code
const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

// Basic fragment shader source code for a red-orange color (#f73416)
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "    FragColor = vec4(0.96f, 0.2f, 0.09f, 1.0f);\n"
    "}\0";

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
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return -1;
  }

  // Makes the OpenGL context in window the current context in this thread
  glfwMakeContextCurrent(window);

  // Make sure GLAD was properly initialized
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  // Set the OpenGL viewport to start at (0, 0) and span 800x600 pixels
  glViewport(0, 0, WIDTH, HEIGHT);

  // Set the callback to call when the GLFW window size changes
  // framebuffer_size_callback resizes the OpenGL viewport accordingly
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Sets up the vertex shader by creating a new OpenGL Vertex Shader object and
  // storing its id in vertexShader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  // Loads a shader's source code into a shader object
  //
  // The first argument is the object to load the code into
  //
  // The second argument is the number of source strings
  //
  // The third argument is an array of pointers to strings containing the source
  // code
  //
  // The fourth argument is an array of string lengths but we can
  // apparently ignore that for now
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  // Compiles the shader stored in vertexShader
  glCompileShader(vertexShader);

  // Compile status for the shaders and a buffer to hold messages
  int success;
  char infoLog[512];

  // Gets information about a shader
  // With the following call, we are getting the shader's compile status and
  // storing it in success
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) { // If not success
    // Get the shader's info log and store it in infoLog
    //
    // The first parameter is the shader itself
    //
    // The second parameter is the maximum length of the info log (infoLog is a
    // array of 512 chars)
    //
    // The third parameter is a pointer to a length
    // describing the returned log length But we don't need that
    //
    // The fourth log is the buffer to write the info log into
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);

    // Just print the info log as a string
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");
    printf("%s", infoLog);
  }

  // Exact same setup as the vertex shader but this time, instead setup a
  // fragment shader
  unsigned int fragShader;
  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragShader);
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
    printf("ERROR::SHADER::FRAG::COMPILATION_FAILED\n");
    printf("%s", infoLog);
  }

  // Sets up the shader program which is the final linked version of multiple
  // shaders
  unsigned int shaderProgram = glCreateProgram();
  // Attach the vertex and fragment shader to the shader program
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragShader);
  // Link the shaders together into a singular shader program
  glLinkProgram(shaderProgram);

  // Check for the linking status of the program
  // This is the same process as checking for the success of compiling a shader
  // but instead, this deals with linking shaders and a shader program
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("ERROR:SHADER::PROGRAM::LINKING_FAILED\n");
    printf("%s", infoLog);
  }

  // Delete the shader objects from memory since now they have been linked into
  // the program and are no longer required separately
  glDeleteShader(vertexShader);
  glDeleteShader(fragShader);

  // Vertices for the rectangle defined as the 4 corners
  float vertices[] = {
      0.5f,  0.5f,  0.0f, // top right
      0.5f,  -0.5f, 0.0f, // bottom right
      -0.5f, -0.5f, 0.0f, // bottom left
      -0.5f, 0.5f,  0.0f, // top left
  };
  // Indices to use when drawing the rectangle
  // Each group of 3 specifies the vertices to draw for each triangle
  // Two triangles = Rectangle
  // Using this allows for points to not have to be repeated in the vertices
  // array as repeating points can be specified in the indices array (1 and 3
  // repeat below)
  unsigned int indices[] = {
      0, 1, 3, // First triangle
      1, 2, 3, // Second triangle
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

  // An EBO is an Element Buffer Object
  // EBOs store indices that OpenGL uses to determine which vertices to draw
  unsigned int EBO;
  // Generate the buffer object and store its id in EBO
  glGenBuffers(1, &EBO);
  // Binds the EBO buffer object to the OpenGL Element Array Buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Copies the indices into the buffer stored as a OpenGL Element Array Buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

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
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  // Enables the vertex attribute at location 0 (as specified in the vertex
  // shader and in the first argument of the glVertexAttribPointer function
  // call)
  glEnableVertexAttribArray(0);

  // For a wireframe drawing, set the mode to GL_LINE rather than GL_FILL
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Sets the shader program that future drawing calls should use
  glUseProgram(shaderProgram);

  // Binds a Vertex Array Object so that the draw call will use the vertex
  // attribute pointer stored in VAO
  glBindVertexArray(VAO);

  // While the GLFW window should not close
  while (!glfwWindowShouldClose(window)) {
    // Process window inputs
    processInput(window);

    // Set the buffer clear color to a off white (#fbf3dc)
    glClearColor(0.98f, 0.95f, 0.86f, 1.0f);
    // Clear the current framebuffer's buffer
    // Because GL_COLOR_BUFFER_BIT is set, the buffer will be cleared to the
    // color defined in the glClearColor function call
    glClear(GL_COLOR_BUFFER_BIT);

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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);

  // Destroys all windows, cursors, and frees all resources
  glfwTerminate();

  // Return success code
  return 0;
}
