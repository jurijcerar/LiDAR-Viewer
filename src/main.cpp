#include <iostream>
#include <string>

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
#include "FileDialog.h"

static constexpr int WIDTH  = 1280;
static constexpr int HEIGHT = 800;

static void uploadPoints(VAO& vao, const PointCloud& pc) {
    vao.Bind();
    VBO vbo(reinterpret_cast<float*>(const_cast<Vertex*>(pc.getVertices().data())),
            sizeof(Vertex) * pc.getVertexCount());
    vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
}

static void uploadEdges(VAO& vao, Graph& graph) {
    vao.Bind();
    VBO vbo(reinterpret_cast<float*>(graph.getVertexData()),
            sizeof(Vertex) * graph.getVertexCount());
    vao.LinkAttribute(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    vao.LinkAttribute(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LiDAR Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_MULTISAMPLE);
    glPointSize(3.0f);

    Shader     shader("shaders/default.vert", "shaders/default.frag");
    Camera     camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));
    PointCloud pointCloud;
    Graph      graph;
    VAO        vaoPoints, vaoLines;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Detect native dialog support ONCE at startup — not per frame
    const bool hasNativeDialog = FileDialog::isAvailable();

    bool        showPoints   = true;
    int         visibleEdges = 0;
    std::string statusMsg;
    char        pathBuf[512] = {};

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    auto doLoad = [&](const std::string& path) {
        if (path.empty()) return;
        try {
            pointCloud.loadFromFile(path);
            uploadPoints(vaoPoints, pointCloud);
            graph.clear();
            visibleEdges = 0;
            auto pos = path.find_last_of("/\\");
            std::string name = (pos == std::string::npos) ? path : path.substr(pos + 1);
            statusMsg = "Loaded: " + name + " (" + std::to_string(pointCloud.getVertexCount()) + " pts)";
        } catch (const std::exception& e) {
            statusMsg = std::string("Error: ") + e.what();
        }
    };

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (!io.WantCaptureMouse && !io.WantCaptureKeyboard)
            camera.Inputs(window, deltaTime);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Activate();
        camera.Matrix(45.0f, 0.00001f, 100.0f, shader, "view", "projection");

        if (showPoints && pointCloud.getVertexCount() > 0) {
            vaoPoints.Bind();
            glDrawArrays(GL_POINTS, 0, pointCloud.getVertexCount());
            vaoPoints.Unbind();
        }

        if (graph.getVertexCount() > 0 && visibleEdges > 0) {
            vaoLines.Bind();
            glDrawArrays(GL_LINES, 0, visibleEdges * 2);
            vaoLines.Unbind();
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(360, 0), ImGuiCond_Once);
        ImGui::Begin("LiDAR Viewer");

        // ── File ─────────────────────────────────────────────────────────────
        ImGui::Separator();
        ImGui::TextDisabled("File");
        ImGui::Separator();

        // Text input + Load always visible
        ImGui::SetNextItemWidth(-80);
        ImGui::InputText("##path", pathBuf, sizeof(pathBuf));
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(70, 0)))
            doLoad(std::string(pathBuf));

        // Browse button only if a native dialog is available
        if (hasNativeDialog) {
            if (ImGui::Button("Browse...", ImVec2(-1, 0))) {
                std::string picked = FileDialog::openFile("Open LAS file", "*.las");
                if (!picked.empty()) {
                    // Copy into pathBuf so the user can see what was picked
                    snprintf(pathBuf, sizeof(pathBuf), "%s", picked.c_str());
                    doLoad(picked);
                }
            }
        }

        if (!statusMsg.empty())
            ImGui::TextWrapped("%s", statusMsg.c_str());

        // ── Rendering ────────────────────────────────────────────────────────
        ImGui::Separator();
        ImGui::TextDisabled("Rendering");
        ImGui::Separator();
        ImGui::Checkbox("Show Points", &showPoints);

        // ── MST ──────────────────────────────────────────────────────────────
        ImGui::Separator();
        ImGui::TextDisabled("MST (Euclidean)");
        ImGui::Separator();

        if (pointCloud.getVertexCount() > 0) {
            static int kNeighbors = 50;
            ImGui::SliderInt("k neighbours", &kNeighbors, 5, 200);

            if (ImGui::Button("Build Kruskal MST", ImVec2(-1, 0))) {
                graph.buildGraph(pointCloud.getPoints(), kNeighbors);
                graph.kruskal();
                visibleEdges = graph.getVertexCount() / 2;
                uploadEdges(vaoLines, graph);
                statusMsg = "MST built: " + std::to_string(visibleEdges) + " edges";
            }

            if (graph.getVertexCount() > 0) {
                ImGui::SliderInt("Visible edges", &visibleEdges, 0, graph.getVertexCount() / 2);
            }
        } else {
            ImGui::TextDisabled("Load a file first.");
        }

        // ── Controls ─────────────────────────────────────────────────────────
        ImGui::Separator();
        ImGui::TextDisabled("Controls");
        ImGui::Separator();
        ImGui::TextDisabled("WASD + Space/LCtrl : move");
        ImGui::TextDisabled("Right mouse drag    : look");

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
