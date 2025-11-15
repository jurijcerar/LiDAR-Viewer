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
    // Camera attributes
    glm::vec3 Position;
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Camera options
    float yaw;
    float pitch;
    float speed;
    float sensitivity;

    // Window
    int width;
    int height;
    bool firstClick;

    // Constructor
    Camera(int width, int height, glm::vec3 position);

    // Generate view & projection matrices and upload to shader
    void Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader,
                const char* uniformView, const char* uniformProjection);

     // Handle keyboard + mouse input
    void Inputs(GLFWwindow* window, float deltaTime);
};

#endif // CAMERA_CLASS_H
