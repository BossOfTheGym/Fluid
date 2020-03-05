#pragma once

#include <Common.h>

#include <Math/MathLib.h>
#include <glWrap/Texture/Texture.h>

namespace res
{
	class Texture2D_Builder
	{
	public:
		using PixelData = std::vector<float>;
		using ImageData = gl::Texture::Image2D_Data;

		using Vec4 = math::Vec4;

		struct SamplingParameters
		{
			gl::TextureParameterValue minFilter = gl::TextureParameterValue::Nearest;
			gl::TextureParameterValue magFilter = gl::TextureParameterValue::Nearest;

			gl::TextureParameterValue wrapModeS = gl::TextureParameterValue::ClampToEdge;
			gl::TextureParameterValue wrapModeT = gl::TextureParameterValue::ClampToEdge;

			Vec4 borderColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
		};


	public:
		Texture2D_Builder(
			const PixelData& pixelDataInit ={}
			, const ImageData& imageDataInit ={}
			, const SamplingParameters& samplingParamsInit ={}
		);

		Texture2D_Builder(
			PixelData&& pixelDataInit
			, const ImageData& imageDataInit ={}
			, const SamplingParameters& samplingParamsInit ={}
		);

		Texture2D_Builder(const Texture2D_Builder&);
		Texture2D_Builder(Texture2D_Builder&&)      noexcept;

		~Texture2D_Builder() = default;

		Texture2D_Builder& operator = (const Texture2D_Builder&);
		Texture2D_Builder& operator = (Texture2D_Builder&&)      noexcept;


	public:
		gl::Texture buildTexture();


	private:
		PixelData          m_pixelData;
		ImageData          m_imageData;
		SamplingParameters m_samplingParams;
	};

}