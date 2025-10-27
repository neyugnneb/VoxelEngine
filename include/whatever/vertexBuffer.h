#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

class VertexBuffer {
    private:
        unsigned int m_vbo;
    public:
        VertexBuffer();
        ~VertexBuffer();

        void bind();
        void fillData(const void *vertices, unsigned int count);  
};



#endif