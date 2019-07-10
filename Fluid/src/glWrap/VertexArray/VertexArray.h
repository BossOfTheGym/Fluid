#pragma once


#include <Common.h>

#include <glWrap/OpenGl.h>
#include <glWrap/ID/Id.h>
#include <glWrap/Buffer/Buffer.h>


class VertexArray : public Id
{
public:
	struct DrawInfo
	{
		DrawMode m_drawMode;      // drawArrays
		GLint    m_first;         // drawArrays

		GLsizei  m_elementsCount; // both drawArrays & drawElements

		DataType m_indicesType;   // drawElements
		GLvoid*  m_indices;       // drawElements
	};

	struct PointerInfo
	{
		GLuint    m_index;      // attribute index
		GLint     m_size;       // number of components per attribute(1,2,3,4)
		DataType  m_type;       // data type
		GLboolean m_normalized; //
		GLsizei   m_stride;     // byte offset between attributes 
		GLvoid*   m_pointer;    // offset of the first element
	};

	
public:
	VertexArray(const DrawInfo& info);

    VertexArray(VertexArray&& vertexBuffer);

    ~VertexArray();


    VertexArray& operator = (VertexArray&& vertexBuffer);


public:
    void bind() const;

	void unbind() const;

    void enableAttribArray(GLuint index);

	void vertexAttribPointer(const PointerInfo& info);

    void disableAttribPointer(GLuint index);

	void drawArrays() const;

	void drawElements() const;

    void deleteArrayBuffer();


public:
    void setAttribPointer(const PointerInfo& ptrInfo);

	void setAttribPointerInBuffer(const Buffer& buffer, const PointerInfo& ptrInfo);

	void setElementsBuffer(const Buffer& buffer);


public:
	DrawInfo& info();

	const DrawInfo& info() const;


private:
    void resetArrayBuffer();


private:
	DrawInfo m_info;
};