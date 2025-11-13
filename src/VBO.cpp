#include"VBO.h"

VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VBO::~VBO() {
    if (glIsBuffer(ID)) {
        glDeleteBuffers(1, &ID);
    }
}

//Better resource management
// Move constructor
VBO::VBO(VBO&& other) noexcept : ID(other.ID) {
    other.ID = 0;
}

// Move assignment
VBO& VBO::operator=(VBO&& other) noexcept {
    if (this != &other) {
        if (glIsBuffer(ID)) glDeleteBuffers(1, &ID);
        ID = other.ID;
        other.ID = 0;
    }
    return *this;
}

void VBO::Bind() {
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
