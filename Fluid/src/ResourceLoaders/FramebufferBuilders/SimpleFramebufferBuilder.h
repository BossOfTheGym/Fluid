#ifndef SIMPLE_FRAMEBUFFER_BULDER_H
#define SIMPLE_FRAMEBUFFER_BULDER_H

#include <glWrap/Framebuffer/Framebuffer.h>


struct FramebufferResources
{
	Framebuffer frame;
	Texture     color;
	Texture     depth;
};

template<class ColorBuilder, class DepthBuilder>
class SimpleFramebufferBuilder
{
public:
	using ColorBuilderType = std::remove_cv_t<std::remove_reference_t<ColorBuilder>>;
	using DepthBuilderType = std::remove_cv_t<std::remove_reference_t<DepthBuilder>>;

public:
	SimpleFramebufferBuilder(
		  ColorBuilder&& colorBuilderInit
		, DepthBuilder&& depthBuilderInit
	)
		: m_colorBuilder(std::forward<ColorBuilder>(colorBuilderInit))
		, m_depthBuilder(std::forward<DepthBuilder>(depthBuilderInit))
	{}

	SimpleFramebufferBuilder(const SimpleFramebufferBuilder&) = default;
	SimpleFramebufferBuilder(SimpleFramebufferBuilder&&)      = default;

	~SimpleFramebufferBuilder() = default;

	SimpleFramebufferBuilder& operator = (const SimpleFramebufferBuilder&) = default;
	SimpleFramebufferBuilder& operator = (SimpleFramebufferBuilder&&)      = default;


public:
	FramebufferResources buildFramebuffer()
	{
		FramebufferResources result;

		result.color = m_colorBuilder.buildTexture();
		result.depth = m_depthBuilder.buildTexture();

		result.frame.createFramebuffer();
		result.frame.namedFramebufferTexture(
			  static_cast<FramebufferAttachment>(FrameBufferAttachmentColor::Color)
			, result.color
			, 0
		);
		result.frame.namedFramebufferTexture(
			  static_cast<FramebufferAttachment>(FramebufferAttachmentDepthStencil::Depth)
			, result.depth
			, 0
		);

		if (result.frame.checkNamedFramebufferStatus(FramebufferTarget::Framebuffer) == FramebufferStatus::Complete)
		{
			std::cout << "Successfully created framebuffer" << std::endl;
		}
		else
		{
			std::cout << "Failed to create framebuffer" << std::endl;
		}

		DrawBuffer buffer = static_cast<DrawBuffer>(FramebufferDrawBuffer::ColorAttachment0);
		result.frame.namedDrawBuffers(1, &buffer);

		return std::move(result);
	}


private:
	ColorBuilderType m_colorBuilder;
	DepthBuilderType m_depthBuilder;
};

#endif