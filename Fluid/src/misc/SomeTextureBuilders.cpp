#include "SomeTextureBuilders.h"

#include <ResourceLoaders/TextureBuilders/Texture2D_Builder.h>

namespace
{
	Texture2D_Builder::PixelData checkedData(int width, int height)
	{
		const int CHANNELS = 4;

		Texture2D_Builder::PixelData result(static_cast<size_t>(width) * height * CHANNELS);

		auto iter = result.begin();
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				*iter = static_cast<float>((i / 32 % 2 == (j / 32 % 2))); ++iter;
				*iter = 0.50f; ++iter;
				*iter =	0.25f; ++iter;
				*iter = 1.00f; ++iter;
			}
		}

		return result;
	}
}

namespace misc
{
	Texture2D_Builder checkedTextureBuilder(int width, int height)
	{
		return Texture2D_Builder(
			  checkedData(width, height)

			, Texture2D_Builder::ImageData{
				  TextureTarget::Texture2D
				, 0
				, InternalFormat::RGBA
				, width
				, height
				, PixelDataFormat::RGBA
				, DataType::Float
				, nullptr
			}

			, Texture2D_Builder::SamplingParameters{
				  TextureParameterValue::Linear
				, TextureParameterValue::Linear
				, TextureParameterValue::ClampToEdge
				, TextureParameterValue::ClampToEdge
			}
		);
	}

	Texture2D_Builder colorTextureBuilder(int width, int height)
	{
		return Texture2D_Builder(
			Texture2D_Builder::PixelData{
			}

			, Texture2D_Builder::ImageData{
				  TextureTarget::Texture2D
				, 0
			    , InternalFormat::RGBA
			    , width
			    , height
			    , PixelDataFormat::RGBA
			    , DataType::Float
			    , nullptr
			}

			, Texture2D_Builder::SamplingParameters{
				  TextureParameterValue::Linear
				, TextureParameterValue::Linear
			    , TextureParameterValue::ClampToEdge
			    , TextureParameterValue::ClampToEdge
			}
		);
	}

	Texture2D_Builder depthTextureBuilder(int width, int height)
	{
		return Texture2D_Builder(
			Texture2D_Builder::PixelData{
			}

			, Texture2D_Builder::ImageData{
				TextureTarget::Texture2D
				, 0
			    , InternalFormat::DepthComponent
			    , width
			    , height
			    , PixelDataFormat::DepthComponent
			    , DataType::Float
			    , nullptr
			}

			, Texture2D_Builder::SamplingParameters{
				  TextureParameterValue::Nearest
				, TextureParameterValue::Nearest
				, TextureParameterValue::ClampToEdge
				, TextureParameterValue::ClampToEdge
			}
		);
	}
}