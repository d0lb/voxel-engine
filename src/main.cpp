#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>   
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Cube.h"

Camera* g_Camera = nullptr;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 800.0 / 2.0;
    static float lastY = 600.0 / 2.0;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    if (g_Camera)
        g_Camera->processMouseMovement(xoffset, yoffset);
}

int main() {
    // GLFW initialization
    if (!glfwInit()) {
        std::cerr << "GLFW init failed" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Voxel Engine", nullptr, nullptr);
    if (!window) {
        std::cerr << "Window creation failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Create objects
    Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    Texture texture("resources/textures/stone.jpg");
    Cube cube;
    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    g_Camera = &camera;

    shader.use();
    shader.setInt("aTexture", 0);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    float lastTime = (float)glfwGetTime();
    float rotationAngle = 0.0f;
    const float rotationSpeed = glm::radians(45.0f);
    const float maxDelta = 0.05f;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        if (deltaTime > maxDelta) deltaTime = maxDelta;
        lastTime += deltaTime;

        // Keyboard input
        glm::vec3 moveDir(0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += camera.getFront();
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= camera.getFront();
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += glm::normalize(glm::cross(camera.getFront(), camera.getUp()));
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir);
            camera.processKeyboardMovement(moveDir, deltaTime);
        }

        // Rotate cube
        rotationAngle += rotationSpeed * deltaTime;
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("uProjection", glm::value_ptr(projection));
        shader.setMat4("uView", glm::value_ptr(camera.getViewMatrix()));
        shader.setMat4("uModel", glm::value_ptr(model));

        texture.bind(0);
        cube.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}