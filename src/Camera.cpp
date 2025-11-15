#include "Camera.h"

// Constructor
Camera::Camera(int width, int height, glm::vec3 position) {
    this->width = width;
    this->height = height;
    this->Position = position;
    this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    this->Orientation = glm::vec3(0.0f, 0.0f, -1.0f);

    this->yaw = -90.0f;
    this->pitch = 0.0f;
    this->speed = 0.8f;
    this->sensitivity = 0.1f;
    this->firstClick = true;
}


// Upload view and projection matrices to shader
void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane,
                    Shader& shader, const char* uniformView, const char* uniformProjection) {

    glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
    glm::mat4 projection = glm::perspective(glm::radians(FOVdeg),
                                            (float)width / height,
                                            nearPlane, farPlane);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniformView), 1, GL_FALSE,
                       glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniformProjection), 1, GL_FALSE,
                       glm::value_ptr(projection));

}

// Process keyboard and mouse input
void Camera::Inputs(GLFWwindow* window, float deltaTime) {
    // Keyboard movement
    glm::vec3 forward = glm::normalize(glm::vec3(Orientation.x, 0.0f, Orientation.z));
    glm::vec3 right = glm::normalize(glm::cross(forward, Up));

    float velocity = speed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        Position += forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        Position -= forward * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        Position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        Position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        Position += Up * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        Position -= Up * velocity;

    // Mouse input (right mouse button)
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        if (firstClick) {
            glfwSetCursorPos(window, width / 2.0, height / 2.0);
            firstClick = false;
        }

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float offsetX = (float)(mouseX - width / 2.0) * sensitivity;
        float offsetY = (float)(height / 2.0 - mouseY) * sensitivity; // inverted Y

        yaw += offsetX;
        pitch += offsetY;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);

        // Update orientation vector
        Orientation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        Orientation.y = sin(glm::radians(pitch));
        Orientation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Orientation = glm::normalize(Orientation);

        // Reset cursor to center
        glfwSetCursorPos(window, width / 2.0, height / 2.0);
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}
