#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

class VAO {
public:
	GLuint ID;

	VAO();
    ~VAO();

	// No copy (can’t have two VAOs managing the same OpenGL object)
    VAO(const VAO&) = delete;
    VAO& operator=(const VAO&) = delete;

    // Move semantics — transfer ownership safely
    VAO(VAO&& other) noexcept;
    VAO& operator=(VAO&& other) noexcept;

	void LinkAttribute(VBO& VBO, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
};
#endif