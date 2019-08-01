#include "Texture2D_Builder.h"


Texture Texture2D_Builder::buildTexture(const Texture::Image2D_Data& data, const SamplingParameters& params)
{
	Texture tex;

	auto& [minFilter, magFilter, wrapModeS, wrapModeT, borderColor] = params;

	tex.bind(TextureTarget::Texture2D);

	tex.texImage2D(data);

	tex.textureParameteri(TextureParameter::TextureMinFilter, static_cast<GLint>(minFilter));
	tex.textureParameteri(TextureParameter::TextureMagFilter, static_cast<GLint>(magFilter));
	tex.textureParameteri(TextureParameter::TextureWrapS, static_cast<GLint>(wrapModeS));
	tex.textureParameteri(TextureParameter::TextureWrapT, static_cast<GLint>(wrapModeT));

	if (wrapModeS == TextureParameterValue::ClampToBorder || wrapModeT == TextureParameterValue::ClampToBorder)
	{
		tex.textureParameterfv(TextureParameter::TextureBorderColor, glm::value_ptr(borderColor));
	}

	tex.unbind(TextureTarget::Texture2D);

	return std::move(tex);
}