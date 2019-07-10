#ifndef consts_h
#define consts_h

#include <GL/glew.h>

enum class ShaderType : GLenum
{
	  Vertex         = GL_VERTEX_SHADER
	, TessControl    = GL_TESS_CONTROL_SHADER
	, TessEvaluation = GL_TESS_EVALUATION_SHADER
	, Geometry       = GL_GEOMETRY_SHADER
	, Fragment       = GL_FRAGMENT_SHADER
	, Compute        = GL_COMPUTE_SHADER

	, None           = static_cast<GLenum>(0)
};

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

enum class DataType : GLenum
{
	  Byte                        = GL_BYTE
	, UnsignedByte                = GL_UNSIGNED_BYTE

	, Short                       = GL_SHORT
	, UnsignedShort               = GL_UNSIGNED_SHORT

	, Int                         = GL_INT
	, UnsignedInt                 = GL_UNSIGNED_INT

	, HalfFloat                   = GL_HALF_FLOAT
	, Float                       = GL_FLOAT
	, Double                      = GL_DOUBLE
	, Fixed                       = GL_FIXED

	, UnsignedInt_2_10_10_10_rev  = GL_UNSIGNED_INT_2_10_10_10_REV
	, UnsignedInt_10F_11F_11F_rev = GL_UNSIGNED_INT_10F_11F_11F_REV

	, None = static_cast<GLenum>(0)
};

enum class Target : GLenum
{
	  ArrayBuffer             = GL_ARRAY_BUFFER
	, AtomicCounterBuffer     = GL_ATOMIC_COUNTER_BUFFER
	, CopyReadBuffer          = GL_COPY_READ_BUFFER
	, CopyWriteBuffer         = GL_COPY_WRITE_BUFFER
	, DispatchIndirectBuffer  = GL_DISPATCH_INDIRECT_BUFFER
	, DrawIndirectBuffer      = GL_DRAW_INDIRECT_BUFFER
	, ElementArrayBuffer      = GL_ELEMENT_ARRAY_BUFFER
	, PixelUnpackBuffer       = GL_PIXEL_UNPACK_BUFFER
	, PixelPackBuffer         = GL_PIXEL_PACK_BUFFER
	, QueryBuffer             = GL_QUERY_BUFFER
	, ShaderStorageBuffer     = GL_SHADER_STORAGE_BUFFER
	, TextureBuffer           = GL_TEXTURE_BUFFER
	, TransformFeedbackBuffer = GL_TRANSFORM_FEEDBACK_BUFFER
	, UniformBuffer           = GL_UNIFORM_BUFFER
	, None                    = static_cast<GLenum>(Id::Empty)
};

enum class Usage : GLenum
{
	  StreamDraw  = GL_STREAM_DRAW
	, StreamRead  = GL_STREAM_READ
	, StreamCopy  = GL_STREAM_COPY
	, StaticDraw  = GL_STATIC_DRAW
	, StaticRead  = GL_STATIC_READ
	, StaticCopy  = GL_STATIC_COPY
	, DynamicDraw = GL_DYNAMIC_DRAW
	, DynamicRead = GL_DYNAMIC_READ
	, DynamicCopy = GL_DYNAMIC_COPY
	, None        = static_cast<GLenum>(0)
};

#endif