#ifndef TEXTURE_2D_BUILDER_H
#define TEXTURE_2D_BUILDER_H


#include <Common.h>

#include <glWrap/Texture/Texture.h>


class Texture2D_Builder
{
public:
	using PixelData = std::vector<float>;
	using ImageData = Texture::Image2D_Data;

	struct SamplingParameters
	{
		TextureParameterValue minFilter = TextureParameterValue::Nearest;
		TextureParameterValue magFilter = TextureParameterValue::Nearest;

		TextureParameterValue wrapModeS = TextureParameterValue::ClampToEdge;
		TextureParameterValue wrapModeT = TextureParameterValue::ClampToEdge;

		Vec4 borderColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};


public:
	Texture2D_Builder(
		  const PixelData& pixelDataInit = {}
		, const ImageData& imageDataInit = {}
		, const SamplingParameters& samplingParamsInit = {}
	);

	Texture2D_Builder(
		  PixelData&& pixelDataInit
		, const ImageData& imageDataInit = {}
		, const SamplingParameters& samplingParamsInit = {}
	);

	Texture2D_Builder(const Texture2D_Builder&);
	Texture2D_Builder(Texture2D_Builder&&)      noexcept;

	~Texture2D_Builder() = default;

	Texture2D_Builder& operator = (const Texture2D_Builder&);
	Texture2D_Builder& operator = (Texture2D_Builder&&)      noexcept;


public:
	Texture buildTexture();


private:
	PixelData          m_pixelData;
	ImageData          m_imageData;
	SamplingParameters m_samplingParams;
};

#endif