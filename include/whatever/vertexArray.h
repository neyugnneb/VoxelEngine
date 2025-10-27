#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
#include <glad/glad.h>

class VertexArray{
    private:
        unsigned int m_vao;
    public:
        VertexArray();
        ~VertexArray();
        void bind();
        void setAttr(unsigned int index, int count, GLenum type, int stride, size_t offset);
};
#endif