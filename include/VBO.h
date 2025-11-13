#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glad/glad.h>

class VBO {
public:
	GLuint ID; //buffer ID

	VBO(GLfloat* vertices, GLsizeiptr size);  // Constructor
    ~VBO();                                   // Destructor

	// Delete copy to avoid double deletion
    VBO(const VBO&) = delete;
    VBO& operator=(const VBO&) = delete;

    // Allow move
    VBO(VBO&& other) noexcept;
    VBO& operator=(VBO&& other) noexcept;

	void Bind();
	void Unbind();
};

#endif
