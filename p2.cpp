#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <iostream>

// 窗口尺寸
const int WIDTH = 800, HEIGHT = 600;

// 变量存储变换参数
glm::vec3 translation(0.0f);
float rotationAngle = 0.0f;
float scaleFactor = 1.0f;

// 顶点着色器
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec3 fragColor;
uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(position, 1.0);
    fragColor = color;
}
)";

// 片段着色器
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 fragColor;
out vec4 color;
void main() {
    color = vec4(fragColor, 1.0);
}
)";

// 金字塔的顶点数据（位置 + 颜色）
GLfloat vertices[] = {
    // 底面
    -0.5f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f,  // 左下红色
     0.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f,  // 右下绿色
     0.5f,  0.0f,  0.5f,  0.0f, 0.0f, 1.0f,  // 右上蓝色
    -0.5f,  0.0f,  0.5f,  1.0f, 1.0f, 0.0f,  // 左上黄色

    // 顶点
     0.0f,  0.8f,  0.0f,  1.0f, 1.0f, 1.0f   // 顶部白色
};

// 索引数据（EBO）
GLuint indices[] = {
    0, 1, 2,   // 底面三角1
    0, 2, 3,   // 底面三角2
    0, 1, 4,   // 侧面1
    1, 2, 4,   // 侧面2
    2, 3, 4,   // 侧面3
    3, 0, 4    // 侧面4
};

// 处理键盘输入
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) translation.y += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) translation.y -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) translation.x -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) translation.x += 0.01f;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) rotationAngle -= glm::radians(5.0f);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) rotationAngle += glm::radians(5.0f);

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) scaleFactor += 0.05f;
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) scaleFactor -= 0.05f;
}

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW 初始化失败!" << std::endl;
        return -1;
    }

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Pyramid", nullptr, nullptr);
    if (!window) {
        std::cerr << "窗口创建失败!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW 初始化失败!" << std::endl;
        return -1;
    }

    // 编译和链接着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 创建 VAO、VBO、EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // 计算变换矩阵
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, translation);
        transform = glm::rotate(transform, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(scaleFactor));

        GLuint transformLoc = glGetUniformLocation(shaderProgram, "transform");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}
