#ifndef TEXTURE_2D_BUILDER_H
#define TEXTURE_2D_BUILDER_H


#include <glWrap/Texture/Texture.h>

#include <Common.h>


class Texture2D_Builder
{
public:
	struct SamplingParameters
	{
		TextureParameterValue minFilter = TextureParameterValue::Nearest;
		TextureParameterValue magFilter = TextureParameterValue::Nearest;

		TextureParameterValue wrapModeS = TextureParameterValue::ClampToEdge;
		TextureParameterValue wrapModeT = TextureParameterValue::ClampToEdge;

		Vec4 borderColor = Vec4(0.0f);
	};


public:
	Texture2D_Builder() = default;

	Texture2D_Builder(const Texture2D_Builder&) = delete;
	Texture2D_Builder(Texture2D_Builder&&)      = delete;

	~Texture2D_Builder() = default;

	Texture2D_Builder& operator = (const Texture2D_Builder&) = delete;
	Texture2D_Builder& operator = (Texture2D_Builder&&)      = delete;


public:
	Texture buildTexture(const Texture::Image2D_Data& data, const SamplingParameters& params);
};

#endif