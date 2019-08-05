#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>
#include <glWrap/Texture/Texture.h>


class Framebuffer : public Id
{
public:
	struct Rectangle
	{
		GLint x0 = static_cast<GLint>(0);
		GLint y0 = static_cast<GLint>(0);
		GLint x1 = static_cast<GLint>(0);
		GLint y1 = static_cast<GLint>(0);
	};

public:
	static Framebuffer sDEFAULT;


public:
	Framebuffer();

	Framebuffer(Framebuffer&& framebuffer);

	~Framebuffer();

	Framebuffer& operator = (Framebuffer&& framebuffer);


public:
	void createFramebuffer();

	void bindFramebuffer(FramebufferTarget target) const;

	void unbindFramebuffer(FramebufferTarget target) const;


	FramebufferStatus checkNamedFramebufferStatus(FramebufferTarget target) const;


	void blitNamedFramebuffer(
		  const Rectangle& destRect
		, const Framebuffer& src
		, const Rectangle& srcRect
		, GLbitfield mask
		, FramebufferFilter filter
	);


	void namedFramebufferParameteri(FramebufferParameteri name, GLint value);

	void namedFramebufferTexture(FramebufferAttachment attachment, const Texture& texture, GLint level);

	void namedDrawBuffers(GLsizei count, const FramebufferAttachment* bufs); // TODO


	void framebufferTextureLayer(FramebufferAttachment attachment, const Texture& texture, GLint level, GLint layer);


	


	void deleteFramebuffer();

private:
	void resetFramebuffer();
};

#endif