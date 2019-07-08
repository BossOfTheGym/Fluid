#pragma once


#include <Common.h>

#include <glWrap/OpenGl.h>
#include <glWrap/ID/Id.h>


class VertexArray : public Id
{
public:
	VertexArray();

    VertexArray(GLsizei elements, GLsizei size, const Float* data);

    VertexArray(VertexArray&& vertexBuffer);

    ~VertexArray();


    VertexArray& operator = (VertexArray&& vertexBuffer);


public:
    void bind() const;

	void unbind() const;

    void setAttribPointer(GLuint index, GLint size, GLenum element, GLsizei stride, const void* offset);

    void enableAttribArray(GLuint index);

    void disableAttribPointer(GLuint index);



    GLuint id() const;

    GLsizei primitivesCount() const;


    void deleteArrayBuffer();


private:
    void resetArrayBuffer();

    bool loadData(GLsizei size, const Float* data);


private:
    GLsizei m_count;
	GLsizei m_elements;
};