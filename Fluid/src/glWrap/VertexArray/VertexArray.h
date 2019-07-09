#pragma once


#include <Common.h>

#include <glWrap/OpenGl.h>
#include <glWrap/ID/Id.h>


class VertexArray : public Id
{
public:
	enum class DrawMode : GLenum
	{
		  Points                 = GL_POINTS

		, LineStrip              = GL_LINE_STRIP
		, LineLoop               = GL_LINE_LOOP
		, Lines                  = GL_LINES
		, LineStripAdjacency     = GL_LINE_STRIP_ADJACENCY
		, LinesAdjacency         = GL_LINES_ADJACENCY

		, TriangleStrip          = GL_TRIANGLE_STRIP
		, TriangleFan            = GL_TRIANGLE_FAN
		, Triangles              = GL_TRIANGLES
		, TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY
		, TrianglesAdjacency     = GL_TRIANGLES_ADJACENCY

		, Patches                = GL_PATCHES

		, None = static_cast<GLenum>(0)
	};

	enum class IndicesType : GLenum
	{
		  UnsignedByte  = GL_UNSIGNED_BYTE
		, UnsignedShort = GL_UNSIGNED_SHORT
		, UnsignedInt   = GL_UNSIGNED_INT

		, None = static_cast<GLenum>(0)
	};


	struct DrawInfo
	{
		//DrawInfo(
		//	  DrawMode    drawMode
		//	, GLint       first
		//	, GLsizei     elementsCount
		//	, IndicesType indicesType
		//	, GLvoid*     indices
		//);

		DrawMode    m_drawMode;      // drawArrays
		GLint       m_first;         // drawArrays

		GLsizei     m_elementsCount; // both drawArrays & drawElements

		IndicesType m_indicesType;   // drawElements
		GLvoid*     m_indices;       // drawElements
	};


public:
	VertexArray(const DrawInfo& info);

    VertexArray(VertexArray&& vertexBuffer);

    ~VertexArray();


    VertexArray& operator = (VertexArray&& vertexBuffer);


public:
    void bind() const;

	void unbind() const;

    void setAttribPointer(GLuint index, GLint size, GLenum element, GLsizei stride, const void* offset);

    void enableAttribArray(GLuint index);

    void disableAttribPointer(GLuint index);

	void drawArrays() const;

	void drawElements() const;

    void deleteArrayBuffer();


public:



public:
	DrawInfo& info();

	const DrawInfo& info() const;


private:
    void resetArrayBuffer();


private:
	DrawInfo m_info;
};