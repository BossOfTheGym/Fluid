#ifndef SOME_TEXTURE_BUILDERS_H
#define SOME_TEXTURE_BUILDERS_H

class Texture2D_Builder;

namespace misc
{
	Texture2D_Builder checkedTextureBuilder(int width, int height);

	Texture2D_Builder colorTextureBuilder(int width, int height);

	Texture2D_Builder depthTextureBuilder(int width, int height);
}

#endif