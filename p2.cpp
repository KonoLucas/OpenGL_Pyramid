#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// Window size
const int WIDTH = 800, HEIGHT = 600;

// Vertex Shader
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 vertexColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)glsl";

// Fragment Shader
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0f);
    }
)glsl";

// Vertex data for the pyramid
GLfloat vertices[] = {
    // Base Triangle 1 (Red)
    -0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 0.0f,  // 0
    -0.5f,  0.0f,  0.5f, 1.0f, 0.0f, 0.0f,  // 1
     0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 0.0f,  // 2

    // Base Triangle 2 (Green)
    -0.5f,  0.0f,  0.5f, 0.0f, 1.0f, 0.0f,  // 1
     0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 0.0f,  // 2
     0.5f,  0.0f,  0.5f, 0.0f, 1.0f, 0.0f,  // 3

    // Side 1 (Blue)
    -0.5f,  0.0f, -0.5f, 0.0f, 0.0f, 1.0f,  // 0
    -0.5f,  0.0f,  0.5f, 0.0f, 0.0f, 1.0f,  // 1
     0.0f,  0.8f,  0.0f, 0.5f, 0.5f, 0.5f,  // 4

    // Side 2 (Yellow)
    -0.5f,  0.0f,  0.5f, 1.0f, 1.0f, 0.0f,  // 1
     0.5f,  0.0f,  0.5f, 1.0f, 1.0f, 0.0f,  // 3
     0.0f,  0.8f,  0.0f, 0.5f, 0.5f, 0.5f,  // 4

    // Side 3 (Cyan)
     0.5f,  0.0f,  0.5f, 0.0f, 1.0f, 1.0f,  // 3
     0.5f,  0.0f, -0.5f, 0.0f, 1.0f, 1.0f,  // 2
     0.0f,  0.8f,  0.0f, 0.5f, 0.5f, 0.5f,  // 4

    // Side 4 (Magenta)
     0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 1.0f,  // 2
    -0.5f,  0.0f, -0.5f, 1.0f, 0.0f, 1.0f,  // 0
     0.0f,  0.8f,  0.0f, 0.5f, 0.5f, 0.5f   // 4
};

// Object transformation variables
glm::vec3 objectPosition = glm::vec3(0.0f); // Object position
float rotationAngle = 0.0f; // Rotation angle
float scaleFactor = 1.0f; // Scale factor

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        objectPosition.y += 0.1f; // Move up (w)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        objectPosition.y -= 0.1f; // Move down (s)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        objectPosition.x -= 0.1f; // Move left (a)
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        objectPosition.x += 0.1f; // Move right (d)

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        rotationAngle -= glm::radians(5.0f);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        rotationAngle += glm::radians(5.0f);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        scaleFactor += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        scaleFactor = std::max(0.05f, scaleFactor - 0.05f);
}

GLuint compileShaders(const char* vertexSource, const char* fragmentSource) {
    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Shader Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed!" << std::endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Pyramid", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable V-Sync

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed!" << std::endl;
        return -1;
    }

    // Compile shaders
    GLuint shaderProgram = compileShaders(vertexShaderSource, fragmentShaderSource);

    // Set up VAO and VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Fixed camera setup
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Camera target
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up direction
    );

    // Fixed projection matrix
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Calculate transformation matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition); // 使用 objectPosition 进行平移
        model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f)); 
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); 
        model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f)); 
        model = glm::scale(model, glm::vec3(scaleFactor));


        // Get uniform locations
        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        // Pass matrices to shader
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw pyramid
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 18);
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}