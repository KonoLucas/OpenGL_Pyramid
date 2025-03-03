#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// Coordinate system: the Z-axis points upwards
// Modified vertex shader to add color input and pass it to the fragment shader
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor; // Add color attribute
    out vec3 vertexColor; // Output color to fragment shader
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        vertexColor = aColor; // Pass color
    }
)glsl";

// Modified fragment shader to use vertex color
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 vertexColor; // Receive color from vertex shader
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0f); // Use vertex color
    }
)glsl";

// Modify processInput function to handle W, S, A, D keys
void processInput(GLFWwindow* window, glm::vec3& translation, float& rotationAngle,
    bool& rotationPending,float& scaleZ) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float moveSpeed = 0.001f; // Movement speed
    const float rotationSpeed = glm::radians(30.0f); // Rotation speed (30 degrees converted to radians)
    // Translation controls
    //Press W -> move up along +ve z-axis
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        translation.z += moveSpeed; 
    //Press S -> move down along -ve z-axis
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        translation.z -= moveSpeed; 
    //Press A -> move horizontally left along +ve x-axis & -ve y-axis
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        translation.x += moveSpeed; 
        translation.y -= moveSpeed;
    }
    //Press D -> move horizontally right along -ve x-axis & +ve y-axis
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        translation.x -= moveSpeed; 
        translation.y += moveSpeed;
    }

    // Rotation controls
     // Press Q -> Counterclockwise rotation by 30° (ensure execution only once)
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !rotationPending) {
        rotationAngle -= 30.0f;
        rotationPending = true; // Mark rotation as completed to avoid repeated execution
    }

    // Press E -> Clockwise rotation by 30° (ensure execution only once)
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !rotationPending) {
        rotationAngle += 30.0f;
        rotationPending = true; // Mark rotation as completed to avoid repeated execution
    }

    // When keys are released, reset rotationPending to allow the next rotation
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        rotationPending = false;
    }

    // Scaling controls
    // Press R -> Scale in the +z direction
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scaleZ += 0.001f; // Increase scale factor (small increment)
        if (scaleZ > 5.0f) scaleZ = 5.0f; // Limit maximum value
    }

    // Press F -> Scale in the -z direction
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        scaleZ -= 0.001f; // Decrease scale factor (small increment)
        if (scaleZ < 0.1f) scaleZ = 0.1f; // Limit minimum value
    }
}

int main() {
   // Initialize GLFW and create a window (unchanged)
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Pyramid with OpenGL", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Define vertex data with color attributes
    float vertices[] = {
        // Position (x, y, z)         // Color (r, g, b)
        -0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f, // Bottom left - Red
         0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f, // Bottom right - Green
         0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // Top right - Blue
        -0.5f,  0.5f, 0.0f,      1.0f, 1.0f, 0.0f, // Top left - Yellow
         0.0f,  0.0f, 1.0f,      1.0f, 0.5f, 0.2f  // Peak - Orange
    };

    unsigned int indices[] = {
        0, 1, 2,  // Base triangle 1
        2, 3, 0,  // Base triangle 2
        0, 1, 4,  // Front face
        1, 2, 4,  // Right face
        2, 3, 4,  // Back face
        3, 0, 4   // Left face
    };

  // Create VBO, VAO, EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

  // Enable depth testing to correctly display 3D shapes
    glEnable(GL_DEPTH_TEST);
    glm::vec3 translation(0.0f, 0.0f, 0.0f);
    float rotationAngle = 0.0f;
    const float rotationSpeed = 50.0f;
    bool rotationPending = false;
    float scaleZ = 1.0f; 
    float lastFrame = 0.0f; 

    while (!glfwWindowShouldClose(window)) {
        //float currentFrame = glfwGetTime();
        //float deltaTime = currentFrame - lastFrame; 
        //lastFrame = currentFrame;
        processInput(window, translation, rotationAngle, rotationPending,scaleZ);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        glUseProgram(shaderProgram);

        // Apply transformations
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, translation); 
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f)); 
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, scaleZ));

        glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), 
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)  
        );

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 transform = projection * view * model;

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup and terminate
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}