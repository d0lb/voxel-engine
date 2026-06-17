#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>   
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Block.h"
#include "World.h"


Camera* g_Camera = nullptr;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastX = 800.0f / 2.0f;
    static float lastY = 600.0f / 2.0f;
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
    //glDisable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");
    Texture atlas("resources/textures/atlas.png");
    Camera camera(glm::vec3(0.0f, 5.0f, 8.0f));
    g_Camera = &camera;

    World world;

    shader.use();
    shader.setInt("uAtlas", 0);  // use texture unit 0

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    float lastTime = (float)glfwGetTime();
    const float maxDelta = 0.05f;

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        if (deltaTime > maxDelta) deltaTime = maxDelta;
        lastTime += deltaTime;

        // Camera movement
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("uProjection", glm::value_ptr(projection));
        shader.setMat4("uView", glm::value_ptr(camera.getViewMatrix()));

        atlas.bind(0);
        world.draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}