#include "SimpleFramebufferBuilder.h"


Framebuffer SimpleFramebufferBuilder::buildFramebuffer(const Texture& color, const Texture& depth)
{
	Framebuffer result;

	result.createFramebuffer();
	result.namedFramebufferTexture(
		static_cast<FramebufferAttachment>(FrameBufferAttachmentColor::Color)
		, color
		, 0
	);
	result.namedFramebufferTexture(
		static_cast<FramebufferAttachment>(FramebufferAttachmentDepthStencil::Depth)
		, depth
		, 0
	);
	if (result.checkNamedFramebufferStatus(FramebufferTarget::Framebuffer) == FramebufferStatus::Complete)
	{
		std::cout << "Successfully created framebuffer" << std::endl;
	}
	else
	{
		std::cout << "Failed to create framebuffer" << std::endl;
	}
	DrawBuffer buffer = static_cast<DrawBuffer>(FramebufferDrawBuffer::ColorAttachment0);
	result.namedDrawBuffers(1, &buffer);

	return result;
}
