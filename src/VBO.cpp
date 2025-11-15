#include"VBO.h"

//VBO stores vertex data (positions, normals, texture coordinates, colors, etc.) 
// directly on the GPU so the graphics card can access it quickly when drawing.

VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID); //create buffer
	glBindBuffer(GL_ARRAY_BUFFER, ID); //make buffer active
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW); //upload data to gpu memmory
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
