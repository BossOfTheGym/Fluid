#include "Framebuffer.h"


//statics
Framebuffer Framebuffer::sDEFAULT = Framebuffer();


//constructors & destructor
Framebuffer::Framebuffer() : Id()
{}

Framebuffer::Framebuffer(Framebuffer&& buffer) : Id()
{
	*this = std::move(buffer);
}

Framebuffer::~Framebuffer()
{
	deleteFramebuffer();
}

//operators
Framebuffer& Framebuffer::operator = (Framebuffer&& buffer)
{
	static_cast<Id&>(*this) = static_cast<Id&&>(buffer);

	return *this;
}

//core functions
void Framebuffer::createFramebuffer()
{
	GLuint framebufferId;

	glCreateFramebuffers(1, &framebufferId);

	static_cast<Id&>(*this) = framebufferId;
}

void Framebuffer::bindFramebuffer(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<GLenum>(target), id());
}

//doesn't require object's state
void Framebuffer::unbindFramebuffer(FramebufferTarget target) const
{
	glBindFramebuffer(static_cast<GLenum>(target), Id::Empty);
}

FramebufferStatus Framebuffer::checkNamedFramebufferStatus(FramebufferTarget target) const
{
	return FramebufferStatus{glCheckNamedFramebufferStatus(id(), static_cast<GLenum>(target))};
}

void Framebuffer::blitNamedFramebuffer(
	  const Rectangle& destRect
	, const Framebuffer& src
	, const Rectangle& srcRect
	, GLbitfield mask
	, FramebufferFilter filter
)
{
	auto& [srcX0, srcY0, srcX1, srcY1] = srcRect;
	auto& [destX0, destY0, destX1, destY1] = destRect;

	glBlitNamedFramebuffer(
		src.id(), id()
		, srcX0, srcY0, srcX1, srcY1
		, destX0, destY0, destX1, destY1
		, mask
		, static_cast<GLenum>(filter)
	);
}


void Framebuffer::namedFramebufferParameteri(FramebufferParameteri name, GLint value)
{
	glNamedFramebufferParameteri(id(), static_cast<GLenum>(name), value);
}

void Framebuffer::namedFramebufferTexture(FramebufferAttachment attachment, const Texture& texture, GLint level)
{
	glNamedFramebufferTexture(id(), static_cast<GLenum>(attachment), texture.id(), level);
}

void Framebuffer::framebufferTextureLayer(FramebufferAttachment attachment, const Texture& texture, GLint level, GLint layer)
{
	//TODO
}

void Framebuffer::namedDrawBuffers(GLsizei count, const FramebufferAttachment* bufs)
{
	glNamedFramebufferDrawBuffers(id(), count, static_cast<const GLenum*>(bufs));
}





void Framebuffer::deleteFramebuffer()
{
	GLuint framebufferID = id();

	glDeleteFramebuffers(1, &framebufferID);
}

void Framebuffer::resetFramebuffer()
{
	resetId();
}