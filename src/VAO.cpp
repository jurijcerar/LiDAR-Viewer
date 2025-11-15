#include"VAO.h"

//VAO is an object that remembers how your vertex data is structured

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

VAO::~VAO() {
    if (glIsVertexArray(ID)) {
        glDeleteVertexArrays(1, &ID);
    }
}

// Better Resource Management
VAO::VAO(VAO&& other) noexcept : ID(other.ID) {
    other.ID = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept {
    if (this != &other) {
        if (glIsVertexArray(ID)) {
            glDeleteVertexArrays(1, &ID);
        }
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}


void VAO::LinkAttribute(VBO& VBO, GLuint layout, GLuint numberOfComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, numberOfComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}
