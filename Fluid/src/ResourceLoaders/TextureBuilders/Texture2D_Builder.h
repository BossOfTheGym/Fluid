#ifndef TEXTURE_2D_BUILDER_H
#define TEXTURE_2D_BUILDER_H


#include <glWrap/Texture/Texture.h>


class Texture2D_Builder
{
public:
	Texture2D_Builder();

	Texture2D_Builder(const Texture2D_Builder&) = delete;
	Texture2D_Builder(Texture2D_Builder&&)      = delete;

	~Texture2D_Builder() = default;

	Texture2D_Builder& operator = (const Texture2D_Builder&) = delete;
	Texture2D_Builder& operator = (Texture2D_Builder&&)      = delete;


public:
	Texture buildTexture();
};

#endif