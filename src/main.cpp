#include<iostream>
#include"imgui.h"
#include"imgui_impl_opengl3.h"
#include"imgui_impl_glfw.h"
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"ShaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"Camera.h"
#include"PointCloud.h"
#include"Graph.h"

#include <iostream>
#include <chrono>

const unsigned int width = 800;
const unsigned int height = 800;

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(width, height, "LIDARViewer", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();

	glViewport(0, 0, width, height);

	Shader shaderProgram("default.vert", "default.frag");

	Graph graph;
	PointCloud pointCloud;

	VAO VAOPoints;

	VAO VAOLines;

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_PROGRAM_POINT_SIZE);

	glEnable(GL_MULTISAMPLE);

	glPointSize(5.0f);

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	char filePath[32] = { 0 };
	bool useEuclid = false;
	bool showPoints = true;
	int numberOfIterations = 0;
	char numberOfPoints[32] = { 0 };

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void) io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.0000001f, 100.0f, shaderProgram, "camMatrix");

		if (showPoints) {
			VAOPoints.Bind();
			glDrawArrays(GL_POINTS, 0, pointCloud.getVerticesCount() * 3);
			VAOPoints.Unbind();
		}
		VAOLines.Bind();
		glDrawArrays(GL_LINES, 0, graph.getVerticesCount() * 6);
		VAOLines.Unbind();

		ImGui::Begin("LIDARViewer");
		ImGui::InputText("File Name", filePath, 32);
		//ImGui::InputText("Number of Points", numberOfPoints, 32);
		if (ImGui::Button("Load")) {
			std::chrono::time_point<std::chrono::system_clock> start, end;

			start = std::chrono::system_clock::now();
			pointCloud.buildPointCloud(filePath, 40000);
			end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed_seconds = end - start;

			std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
			
			graph.clearGraph();
			numberOfIterations = 0;
			VAOPoints.Bind();

			VBO VBOPoints((float*)pointCloud.getVerticesData(), sizeof(Vertice) * pointCloud.getVerticesCount());
			VAOPoints.LinkAttribute(VBOPoints, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
			VAOPoints.LinkAttribute(VBOPoints, 1, 3, GL_FLOAT, 3 * sizeof(float), (void*) (3 * sizeof(float)));
			VAOPoints.Unbind();
			VBOPoints.Unbind();
		}

		ImGui::Checkbox("Use Euclid distance", &useEuclid);
		if (ImGui::Button("Kruskal")) {
			// Using time point and system_clock
			std::chrono::time_point<std::chrono::system_clock> start, end;

			start = std::chrono::system_clock::now();
			graph.buildGraph(useEuclid, pointCloud.getVerticesCount());
			end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed_seconds = end - start;

			std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

			start = std::chrono::system_clock::now();
			graph.kruskal();
			end = std::chrono::system_clock::now();
			elapsed_seconds = end - start;

			std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
			
			numberOfIterations = graph.getVerticesCount()/2;

		}

		if (graph.getVerticesCount() != 0) {
			ImGui::Checkbox("Show Points", &showPoints);
			ImGui::SliderInt("Number of Iterations", &numberOfIterations, 1, (graph.getVerticesCount()/2));
			VAOLines.Bind();

			VBO VBOLines((float*)graph.getVerticesData(), sizeof(Vertice) * numberOfIterations * 2);
			//VBO VBOLines((float*)graph.getVerticesData(), sizeof(Vertice)* graph.getVerticesCount());
			VAOLines.LinkAttribute(VBOLines, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
			VAOLines.LinkAttribute(VBOLines, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			VAOLines.Unbind();
			VBOLines.Unbind();
		}
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