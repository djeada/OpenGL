#include <GL/glew.h>

struct Vertex {
	float x;
	float y;
	float z;

	float r;
	float g;
	float b;
	float a;
};

class VertexBuffer {
	
public:
    VertexBuffer(void* data, int numVertices);
    virtual ~VertexBuffer();
    void bind();
    void unbind();

private:
    GLuint bufferId;
    GLuint vao;
};
