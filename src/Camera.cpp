#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
    : width(width), height(height), Position(position),
      Up(glm::vec3(0.0f, 1.0f, 0.0f)), Orientation(glm::vec3(0.0f, 0.0f, -1.0f)),
      Yaw(-90.0f), Pitch(0.0f)
{
    Right = glm::normalize(glm::cross(Orientation, Up));
}

void Camera::UpdateMatrix(float FOVdeg, float nearPlane, float farPlane, const Shader& shader, const char* uniform) {
    glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    GLint uniformLoc = glGetUniformLocation(shader.ID, uniform);
    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::ProcessInputs(GLFWwindow* window, float deltaTime) {
    float velocity = speed * deltaTime;

    // Recalculate Right vector
    Right = glm::normalize(glm::cross(Orientation, Up));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) Position += velocity * Orientation;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) Position -= velocity * Orientation;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) Position -= velocity * Right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) Position += velocity * Right;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) Position += velocity * Up;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) Position -= velocity * Up;

    // Mouse input
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick) {
            glfwSetCursorPos(window, width / 2.0, height / 2.0);
            firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float offsetX = (mouseX - width / 2.0f) * sensitivity;
        float offsetY = (height / 2.0f - mouseY) * sensitivity; // reversed Y

        Yaw += offsetX;
        Pitch += offsetY;

        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        UpdateOrientation();

        glfwSetCursorPos(window, width / 2.0, height / 2.0);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}

void Camera::UpdateOrientation() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Orientation = glm::normalize(front);
}
