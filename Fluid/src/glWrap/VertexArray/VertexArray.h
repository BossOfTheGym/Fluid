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

		IndicesType indicesType = IndicesType::None; // drawElements
		GLvoid*  indices        = nullptr;           // drawElements
	};

	//TODO : passing data of type 'double' erquires some special logic(good practice)
	struct PointerInfo
	{
		GLuint        index      = static_cast<GLuint>(0);  // attribute index
		AttributeSize size       = AttributeSize::One;      // number of components per attribute(1,2,3,4)
		DataType      type       = DataType::None;          // data type
		GLBool        normalized = GLBool::False;           //
		GLsizei       stride     = static_cast<GLsizei>(0); // byte offset between attributes 
		GLvoid*       pointer    = nullptr;                 // offset of the first element
	};

	
public:
	VertexArray();

    VertexArray(VertexArray&& vertexBuffer) noexcept;

    ~VertexArray();


    VertexArray& operator = (VertexArray&& vertexBuffer) noexcept;


public:
	void genArray();

	void createArray();

    void bind() const;

	void unbind() const;

    void enableAttribArray(GLuint index);

	void vertexAttribPointer(const PointerInfo& info);

    void disableAttribArray(GLuint index);

	void drawArrays() const;

	void drawElements() const;

    void deleteArrayBuffer();


public:
	void draw() const;

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