#ifndef SIMPLE_FRAMEBUFFER_BULDER_H
#define SIMPLE_FRAMEBUFFER_BULDER_H

#include <glWrap/Framebuffer/Framebuffer.h>

class SimpleFramebufferBuilder
{
public:
	SimpleFramebufferBuilder() = default;
	SimpleFramebufferBuilder(const SimpleFramebufferBuilder&) = delete;
	SimpleFramebufferBuilder(SimpleFramebufferBuilder&&)      = delete;

	~SimpleFramebufferBuilder() = default;

	SimpleFramebufferBuilder& operator = (const SimpleFramebufferBuilder&) = delete;
	SimpleFramebufferBuilder& operator = (SimpleFramebufferBuilder&&)      = delete;


public:
	Framebuffer buildFramebuffer(const Texture& color, const Texture& depth);
};

#endif