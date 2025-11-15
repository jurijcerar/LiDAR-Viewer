#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "Camera.h"
#include "PointCloud.h"
#include "Graph.h"

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 800;

int main() {

	 // --- Initialize GLFW ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LIDARViewer", nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
	glfwMakeContextCurrent(window);
	gladLoadGL();

	glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);
    glPointSize(5.0f);

	// --- Shader, camera, and objects ---
    Shader shader("shaders/default.vert", "shaders/default.frag");
    Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));
    PointCloud pointCloud;
    Graph graph;

    VAO VAOPoints;
    VAO VAOLines;

	 // --- ImGui Setup ---
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    char filePath[128] = {0};
    bool useEuclid = false;
    bool showPoints = true;
    int numberOfIterations = 0;

	float deltaTime = 0.0f;
    float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
        // --- Frame timing ---
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

		 // --- Input ---
        camera.Inputs(window, deltaTime);

        // --- Render ---
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 shader.Activate();
        camera.Matrix(45.0f, 0.00001f, 100.0f, shader, "view", "projection");

        // Draw points
        if (showPoints && pointCloud.getVerticesCount() > 0) {
            VAOPoints.Bind();
            glDrawArrays(GL_POINTS, 0, pointCloud.getVerticesCount());
            VAOPoints.Unbind();
        }

		// Draw lines
        if (graph.getVerticesCount() > 0) {
            VAOLines.Bind();
            glDrawArrays(GL_LINES, 0, numberOfIterations * 2);
            VAOLines.Unbind();
        }

		// --- ImGui ---
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("LIDARViewer");
        ImGui::InputText("File Name", filePath, 128);

        if (ImGui::Button("Load")) {
            pointCloud.loadFromFile(filePath, 120000);

            // Upload points to GPU
            VAOPoints.Bind();
            VBO vboPoints((float*)pointCloud.getVertices().data(),
                          sizeof(Vertice) * pointCloud.getVerticesCount());
            VAOPoints.LinkAttribute(vboPoints, 0, 3, GL_FLOAT, sizeof(Vertice), (void*)0);
            VAOPoints.LinkAttribute(vboPoints, 1, 3, GL_FLOAT, sizeof(Vertice), (void*)(3 * sizeof(float)));
            VAOPoints.Unbind();
            vboPoints.Unbind();

            graph.clearGraph();
            numberOfIterations = 0;
        }

		ImGui::Checkbox("Use Euclid distance", &useEuclid);
        if (ImGui::Button("Kruskal") && pointCloud.getVerticesCount() > 0) {
            graph.buildGraph(pointCloud.getPoints(),50, useEuclid);
            graph.kruskal();
            numberOfIterations = graph.getVerticesCount() / 2;

            // Upload edges to GPU
            VAOLines.Bind();
            VBO vboLines((float*)graph.getVerticesData(), sizeof(Vertice) * graph.getVerticesCount());
            VAOLines.LinkAttribute(vboLines, 0, 3, GL_FLOAT, sizeof(Vertice), (void*)0);
            VAOLines.LinkAttribute(vboLines, 1, 3, GL_FLOAT, sizeof(Vertice), (void*)(3 * sizeof(float)));
            VAOLines.Unbind();
            vboLines.Unbind();
        }

		ImGui::Checkbox("Show Points", &showPoints);
        if (graph.getVerticesCount() > 0) {
            ImGui::SliderInt("Number of Iterations", &numberOfIterations, 1, graph.getVerticesCount() / 2);
        }

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

	 // --- Cleanup ---
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}