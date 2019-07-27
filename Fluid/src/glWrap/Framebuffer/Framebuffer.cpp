#include "Framebuffer.h"


//statics
namespace
{
	Framebuffer createDefaultFramebuffer()
	{
		Framebuffer result;

		//framebuffer with empty(zero) id is always considered default
		result.deleteFramebuffer();

		return result;
	}
}

Framebuffer Framebuffer::sDEFAULT = createDefaultFramebuffer();


//constructors & destructor
Framebuffer::Framebuffer()
{
	//TODO
}

Framebuffer::Framebuffer(Framebuffer&& buffer)
{
	//TODO
}

Framebuffer::~Framebuffer()
{
	deleteFramebuffer();
}

//operators
Framebuffer& Framebuffer::operator = (Framebuffer&& buffer)
{
	//TODO

	return *this;
}

//core functions
void Framebuffer::bindFramebuffer(FramebufferTarget target) const
{
	//TODO
}

void Framebuffer::unbindFramebuffer(FramebufferTarget target) const
{
	//TODO
}

FramebufferStatus Framebuffer::checkNamedFramebufferStatus(FramebufferTarget target) const
{
	//TODO

	return FramebufferStatus::Complete;
}

void Framebuffer::blitNamedFramebuffer(
	  const Rectangle& destRect
	, const Framebuffer& src
	, const Rectangle& srcRect
	, GLbitfield mask
	, FramebufferFilter filter
)
{
	//TODO
}


void Framebuffer::namedFramebufferParameteri(FramebufferParameteri name, GLint value)
{
	//TODO
}

void Framebuffer::namedFramebufferTexture(FramebufferAttachment attachment, const Texture& texture, GLint level)
{
	//TODO
}

void Framebuffer::framebufferTextureLayer(FramebufferAttachment attachment, const Texture& texture, GLint level, GLint layer)
{
	//TODO
}

void Framebuffer::namedDrawBuffers(GLsizei count, const FramebufferAttachment* bufs)
{
	//TODO
}

void Framebuffer::deleteFramebuffer()
{
	//TODO
}