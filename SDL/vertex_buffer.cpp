#include "vertex_buffer.h"

VertexBuffer::VertexBuffer(void* data, int numVertices) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(struct Vertex,x));

	glBindVertexArray(0);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &bufferId);
}

void VertexBuffer::bind() {
	glBindVertexArray(vao);
}

void VertexBuffer::unbind() {
	glBindVertexArray(0);
}

