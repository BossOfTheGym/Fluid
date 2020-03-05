#pragma once

#include <glWrap/Framebuffer/Framebuffer.h>
#include <glWrap/Texture/Texture.h>


namespace res
{
	struct FramebufferResources
	{
		gl::Framebuffer frame;
		gl::Texture     color;
		gl::Texture     depth;
	};

	template<class ColorBuilder, class DepthBuilder>
	class SimpleFramebufferBuilder
	{
	public:
		using ColorBuilderType = std::remove_cv_t<std::remove_reference_t<ColorBuilder>>;
		using DepthBuilderType = std::remove_cv_t<std::remove_reference_t<DepthBuilder>>;

	public:
		SimpleFramebufferBuilder(
			ColorBuilder colorBuilderInit
			, DepthBuilder depthBuilderInit
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
				static_cast<gl::FramebufferAttachment>(gl::FrameBufferAttachmentColor::Color)
				, result.color
				, 0
			);
			result.frame.namedFramebufferTexture(
				static_cast<gl::FramebufferAttachment>(gl::FramebufferAttachmentDepthStencil::Depth)
				, result.depth
				, 0
			);

			auto status = result.frame.checkNamedFramebufferStatus(gl::FramebufferTarget::Framebuffer);
			if (status == gl::FramebufferStatus::Complete)
			{
				std::cout << "Successfully created framebuffer" << std::endl;
			}
			else
			{
				std::cout << "Failed to create framebuffer" << std::endl;
			}

			gl::DrawBuffer buffer = static_cast<gl::DrawBuffer>(gl::FramebufferDrawBuffer::ColorAttachment0);
			result.frame.namedDrawBuffers(1, &buffer);

			return result;
		}


	private:
		ColorBuilderType m_colorBuilder;
		DepthBuilderType m_depthBuilder;
	};
}
