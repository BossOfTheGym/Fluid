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
	if (result.checkNamedFramebufferStatus() != FramebufferStatus::Complete)
	{
		std::cout << "Failed to create framebuffer" << std::endl;
	}

	return result;
}
