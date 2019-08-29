#pragma once

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>


class Texture : public Id
{
public:
	struct Image2D_Data
	{
		TextureTarget   target         = TextureTarget::None;
		GLint           level          = static_cast<GLint>(0);
		InternalFormat  internalFormat = InternalFormat::None;
		GLsizei         width          = static_cast<GLsizei>(0);
		GLsizei         height         = static_cast<GLsizei>(0);
		//border = 0
		PixelDataFormat format         = PixelDataFormat::None;
		DataType        type           = DataType::None;
		const GLvoid*   pixels         = nullptr;
	};


public:
	Texture();

    Texture(Texture&& tex) noexcept;

    ~Texture();
	
    Texture& operator = (Texture&& tex) noexcept;


public:
	void genTexture();

    void bind(TextureTarget target) const;

	void unbind(TextureTarget target) const;

	void active(TextureActive texture) const;

	void bindToUnit(TextureUnit unit) const;

    void texParameteri(TextureTarget target, TextureParameter name, GLint parameter);

	void textureParameteri(TextureParameter name, GLint parameter);

	void textureParameterfv(TextureParameter name, const GLfloat* params);

	void texImage2D(const Image2D_Data& data);

    void deleteTexture();


private:
	void resetTexture();
};