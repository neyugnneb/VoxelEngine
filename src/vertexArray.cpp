#include <whatever/vertexArray.h>
#include <glad/glad.h>


VertexArray::VertexArray()
{
    glGenVertexArrays(1,&m_vao);
    bind();

}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_vao);

}

void VertexArray::bind()
{
    glBindVertexArray(m_vao);
}

void VertexArray::setAttr(unsigned int index, int count, GLenum type, int stride, size_t offset)
{
    glVertexAttribPointer(index, count, type, GL_FALSE, stride, (void*)(offset));
    glEnableVertexAttribArray(index);
}