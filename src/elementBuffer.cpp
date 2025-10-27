#include <whatever/elementBuffer.h>
#include <glad/glad.h>

ElementBuffer::ElementBuffer()
{
    glGenBuffers(1,&m_ebo);
    bind();

}

void ElementBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_ebo);
}

void ElementBuffer::fillData(const void *indicies, unsigned int count)
{
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,count , indicies, GL_STATIC_DRAW);

}