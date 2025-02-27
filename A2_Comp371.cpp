#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
// 坐标系：向上的是z轴
// 修改后的顶点着色器，添加颜色输入并传递到片段着色器
const char* vertexShaderSource = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor; // 添加颜色属性
    out vec3 vertexColor; // 输出颜色给片段着色器
    uniform mat4 transform;
    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        vertexColor = aColor; // 传递颜色
    }
)glsl";

// 修改后的片段着色器，使用顶点颜色
const char* fragmentShaderSource = R"glsl(
    #version 330 core
    in vec3 vertexColor; // 从顶点着色器接收颜色
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0f); // 使用顶点颜色
    }
)glsl";

// 修改 processInput 函数以处理 W、S、A、D 键
void processInput(GLFWwindow* window, glm::vec3& translation, float& rotationAngle,
    bool& rotationPending,float& scaleZ) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float moveSpeed = 0.001f; // 移动速度
    const float rotationSpeed = glm::radians(30.0f); // 旋转速度（30度转弧度）
    // W: 前移 (Z 轴负方向), S: 后移 (Z 轴正方向), A: 左移 (X 轴负方向), D: 右移 (X 轴正方向)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        translation.z -= moveSpeed; // 前移
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        translation.z += moveSpeed; // 后移
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        translation.x -= moveSpeed; // 左移
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        translation.x += moveSpeed; // 右移


     // 按 Q 键 -> 逆时针旋转 30°（但确保只执行一次）
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !rotationPending) {
        rotationAngle -= 30.0f;
        rotationPending = true; // 标记已旋转，避免重复执行
    }

    // 按 E 键 -> 顺时针旋转 30°（但确保只执行一次）
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !rotationPending) {
        rotationAngle += 30.0f;
        rotationPending = true; // 标记已旋转，避免重复执行
    }

    // 当按键释放时，重置 rotationPending，允许下次旋转
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
        rotationPending = false;
    }

    // 按 R 键 -> 在 +z 方向缩放
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scaleZ += 0.001f; // 增加缩放因子（更小的增量）
        if (scaleZ > 5.0f) scaleZ = 5.0f; // 限制最大值
    }

    // 按 F 键 -> 在 -z 方向缩放
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        scaleZ -= 0.001f; // 减少缩放因子（更小的增量）
        if (scaleZ < 0.1f) scaleZ = 0.1f; // 限制最小值
    }
}

int main() {
    // 初始化 GLFW 和窗口（保持不变）
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

    // 定义顶点数据，添加颜色属性
    float vertices[] = {
        // 位置 (x, y, z)         // 颜色 (r, g, b)
        -0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f, // 左下角 - 红色
         0.5f, -0.5f, 0.0f,      0.0f, 1.0f, 0.0f, // 右下角 - 绿色
         0.5f,  0.5f, 0.0f,      0.0f, 0.0f, 1.0f, // 右上角 - 蓝色
        -0.5f,  0.5f, 0.0f,      1.0f, 1.0f, 0.0f, // 左上角 - 黄色
         0.0f,  0.0f, 1.0f,      1.0f, 0.5f, 0.2f  // 顶部 - 橙色
    };

    unsigned int indices[] = {
        0, 1, 2,  // 底面三角形 1
        2, 3, 0,  // 底面三角形 2
        0, 1, 4,  // 前面
        1, 2, 4,  // 右面
        2, 3, 4,  // 后面
        3, 0, 4   // 左面
    };

    // 创建 VBO、VAO、EBO
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点属性指针：位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 设置顶点属性指针：颜色
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 编译和链接着色器
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

    // 启用深度测试以正确显示 3D 形状
    glEnable(GL_DEPTH_TEST);
    // 定义平移向量
    glm::vec3 translation(0.0f, 0.0f, 0.0f);
    float rotationAngle = 0.0f;//旋转角度
    const float rotationSpeed = 50.0f;
    bool rotationPending = false;
    float scaleZ = 1.0f; // 初始缩放因子
    float lastFrame = 0.0f; // 记录上一帧时间
    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        //float currentFrame = glfwGetTime();
        //float deltaTime = currentFrame - lastFrame; // 计算时间差
        //lastFrame = currentFrame;
        processInput(window, translation, rotationAngle, rotationPending,scaleZ);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除深度缓冲区

        glUseProgram(shaderProgram);

        // 创建变换矩阵（包含投影）
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, translation); // 应用平移
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f)); // 绕Z轴旋转
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, scaleZ));

        glm::mat4 view = glm::lookAt(
            glm::vec3(2.0f, 2.0f, 2.0f), // 相机位置
            glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
            glm::vec3(0.0f, 0.0f, 1.0f)  // 上方向
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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}