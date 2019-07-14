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
		DrawMode drawMode;      // drawArrays
		GLint    first;         // drawArrays

		GLsizei  elementsCount; // both drawArrays & drawElements

		DataType indicesType;   // drawElements
		GLvoid*  indices;       // drawElements
	};

	struct PointerInfo
	{
		GLuint    index;      // attribute index
		GLint     size;       // number of components per attribute(1,2,3,4)
		DataType  type;       // data type
		GLboolean normalized; //
		GLsizei   stride;     // byte offset between attributes 
		GLvoid*   pointer;    // offset of the first element
	};

	
public:
	VertexArray(const DrawInfo& info = DrawInfo{DrawMode::None, 0, 0, DataType::None, nullptr});

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