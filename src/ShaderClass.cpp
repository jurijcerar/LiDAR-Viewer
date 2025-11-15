#include "ShaderClass.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

std::string getFileContents(const char* fileName) { //get content of shader files
	std::ifstream file(fileName, std::ios::binary);

	if (!file) {
        throw std::runtime_error(std::string("Failed to open file: ") + fileName);
    }

	std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) {
	
	//create shaders
	std::string vertexCode = getFileContents(vertexFile);
	std::string fragmentCode = getFileContents(fragmentFile);

	//load shaders
	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	//compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, nullptr);
	glCompileShader(vertexShader);
	compileErrors(vertexShader, "VERTEX");

	//compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	//create and link shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	compileErrors(ID, "PROGRAM");

	//delete after linking
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

Shader::~Shader() {
    if (glIsProgram(ID)) {
        glDeleteProgram(ID);
    }
}

//Better Memory Management
Shader::Shader(Shader&& other) noexcept : ID(other.ID) {
    other.ID = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (glIsProgram(ID)) glDeleteProgram(ID);
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}

void Shader::Activate() {
	glUseProgram(ID);
}


void Shader::compileErrors(unsigned int shader, const char* type) {
	GLint success;
	char infoLog[1024];

	if (type != "PROGRAM") {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "SHADER_COMPILATION_ERROR for " << type << "\n" << infoLog << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
			std::cerr << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}