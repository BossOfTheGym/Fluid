#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Framebuffer : public Id
{
public:
	struct Rectangle
	{
		GLint x0;
		GLint y0;
		GLint x1;
		GLint y1;
	};


public:
	Framebuffer();
	Framebuffer(Framebuffer&& framebuffer);

	~Framebuffer();

	Framebuffer& operator = (Framebuffer&& framebuffer);


public:
	void bindFramebuffer(FramebufferTarget target) const;

	void unbindFramebuffer(FramebufferTarget target) const;


	FramebufferStatus checkNamedFramebufferStatus(FramebufferTarget target) const;

	void blitNamedFramebuffer(const Framebuffer& src, const Rectangle& srcRect, const Rectangle& destRect);


	void framebufferParameteri();

	void framebufferTexture();

	void framebufferTextureLayer();

	void drawBuffer();


	void deleteFramebuffer();
};

#endif