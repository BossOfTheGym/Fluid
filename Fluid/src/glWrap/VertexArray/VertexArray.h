#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H


#include <glWrap/OpenGl.h>
#include <glWrap/Buffer/Buffer.h>


class VertexArray : public Id
{
public:
	struct DrawInfo
	{
		DrawMode drawMode = DrawMode::None;        // drawArrays
		GLint    first    = static_cast<GLint>(0); // drawArrays

		GLsizei  elementsCount = static_cast<GLsizei>(0); // both drawArrays & drawElements

		DataType indicesType = DataType::None; // drawElements
		GLvoid*  indices     = nullptr;        // drawElements
	};

	struct PointerInfo
	{
		GLuint    index = static_cast<GLuint>(0);       // attribute index
		GLint     size  = static_cast<GLint>(0);        // number of components per attribute(1,2,3,4)
		DataType  type  = DataType::None;               // data type
		GLBool    normalized = GLBool::True;            //
		GLsizei   stride     = static_cast<GLsizei>(0); // byte offset between attributes 
		GLvoid*   pointer    = nullptr;                 // offset of the first element
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

#endif