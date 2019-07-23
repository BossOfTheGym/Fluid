#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>

class Framebuffer : public Id
{
public:
	Framebuffer();
	Framebuffer(Framebuffer&& framebuffer);

	~Framebuffer();

	Framebuffer& operator = (Framebuffer&& framebuffer);


public:
	void bindFramebuffer();

	void unbindFramebuffer();

	void blitFramebuffer();

	void framebufferParameteri();

	void framebufferTexture();

	void framebufferTextureLayer();

	void drawBuffer();


	void deleteFramebuffer();
};

#endif