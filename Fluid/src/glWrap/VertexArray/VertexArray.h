#pragma once


#include <Common.h>

#include <glWrap/Consts.h>


class VertexArray
{
public:
	VertexArray();

    VertexArray(GLsizei elements, GLsizei size, const Float* data);

    VertexArray(const VertexArray& vertexBuffer) = delete;
    VertexArray(VertexArray&& vertexBuffer);

    ~VertexArray();

    VertexArray& operator = (const VertexArray& vertexBuffer) = delete;
    VertexArray& operator = (VertexArray&& vertexBuffer);



    void bind() const;

	void subData(GLintptr offset, GLsizeiptr size, const GLvoid* data);

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
    GLuint m_vertexArrayId;
    GLuint m_arrayBufferId;
	GLuint m_elementsBufferId;

	//TODO
	GLenum m_mode;

    GLsizei m_count;
	GLsizei m_elements;
};