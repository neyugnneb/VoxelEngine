#include <whatever/vertexBuffer.h>
#include <glad/glad.h>

VertexBuffer::VertexBuffer() {
    glGenBuffers(1, &m_vbo);
    bind();
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_vbo);
}

void VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
}

void VertexBuffer::fillData(const void*vertices, unsigned int count) {
    bind();
    glBufferData(GL_ARRAY_BUFFER, count,vertices, GL_STATIC_DRAW);
}