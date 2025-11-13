#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>

std::string getFileContents(const char* fileName);

class Shader {
public:
	GLuint ID;

	// Constructor: load, compile, and link shaders
    Shader(const char* vertexFile, const char* fragmentFile);

    // Destructor: deletes the program automatically
    ~Shader();

	 // Delete copy to prevent double deletion
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Allow move semantics
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

	void Activate();

private:
	void compileErrors(unsigned int shader, const char* type);
};


#endif