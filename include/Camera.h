#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderClass.h"

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Orientation;
    glm::vec3 Up;
    glm::vec3 Right;

    float Yaw;
    float Pitch;

    int width;
    int height;

    bool firstClick = true;

    float speed = 5.0f;         // Units per second
    float sensitivity = 0.1f;   // Mouse sensitivity

    Camera(int width, int height, glm::vec3 position);

    void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane, const Shader& shader, const char* uniform);
    void ProcessInputs(GLFWwindow* window, float deltaTime);

private:
    void UpdateOrientation();
};

#endif
