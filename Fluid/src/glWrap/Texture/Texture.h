#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>


class Texture : public Id
{
public:
	struct Image2D_Data
	{
		TextureTarget  target;
		GLint          level;
		GLint          internalFormat;
		GLsizei        width;
		GLsizei        height;
		//border = 0
		PixelFormat    format;
		DataType       type;
		const GLvoid*  pixels;
	};


public:
	Texture();

    Texture(Texture&& tex);

    ~Texture();
	
    Texture& operator = (Texture&& tex);


public:
    void bind(TextureTarget target) const;

	void unbind(TextureTarget target) const;

	void active(GLenum texture) const;

    void texParameteri(TextureTarget target, TextureParameter name, GLint parameter);

	void textureParameteri(TextureParameter name, GLint parameter);

	void texImage2D(const Image2D_Data& data);


    void deleteTexture();
};