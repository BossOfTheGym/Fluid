#include "Texture2D.h"

#include <FreeImage/FreeImage.h>

//statics
void Texture2D::active(GLenum texture)
{
    glActiveTexture(texture);
}


//constructors & destructor
namespace
{
	Id genTexture()
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		
		return textureId;
	}
}

Texture2D::Texture2D()
	: Id()
{}

Texture2D::Texture2D(const String& location)
{
	static_cast<Id&>(*this) = genTexture();

	glBindTexture(GL_TEXTURE_2D, id());
    if (!valid() || !loadFromLocation(location))
    {
        deleteTexture();
    }
}

Texture2D::Texture2D(int width, int height, const GLubyte* data)
{
	static_cast<Id&>(*this) = genTexture();

    glBindTexture(GL_TEXTURE_2D, id());
    if (!valid() || !loadFromData(width, height, data))
    {
        deleteTexture();
    }
}



Texture2D::Texture2D(Texture2D&& texture)
{
	*this = std::move(texture);
}


Texture2D::~Texture2D()
{
    deleteTexture();
}


//operators
Texture2D& Texture2D::operator = (Texture2D&& texture)
{
	static_cast<Id&>(*this) = static_cast<Id&&>(texture);

    return *this;
}


//core functions
void Texture2D::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id());
}

void Texture2D::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, Id::Empty);
}


void Texture2D::texParameteri(GLenum name, GLint parameter) const
{
    glTexParameteri(GL_TEXTURE_2D, name, parameter);
}


//delete
void Texture2D::deleteTexture()
{
	GLuint textureId = id();
    glDeleteTextures(1, &textureId);

    resetTexture();
}


//load
bool Texture2D::loadFromLocation(const String& location)
{
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(location.c_str(), 0);
    if (format == -1)
    {
        std::cout << "Could not find image" << std::endl;
        return false;
    }
    if (format == FIF_UNKNOWN)
    {
        std::cout << "Couldn't determine file format - attempting to get from file extension..." << std::endl;

        format = FreeImage_GetFIFFromFilename(location.c_str());

        if (!FreeImage_FIFSupportsReading(format))
        {
            std::cout << "Detected image format cannot be read!" << std::endl;
            return false;
        }
    }

    FIBITMAP* bitmap = FreeImage_Load(format, location.c_str());
    int bitsPerPixel = FreeImage_GetBPP(bitmap);

    FIBITMAP* bitmap32;
    if (bitsPerPixel == 32)
    {
        std::cout << "Source image has " << bitsPerPixel << " bits per pixel. Skipping conversion." << std::endl;
        bitmap32 = bitmap;
    }
    else
    {
        std::cout << "Source image has " << bitsPerPixel << " bits per pixel. Converting to 32-bit colour." << std::endl;
        bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
    }

    int imageWidth = FreeImage_GetWidth(bitmap32);
    int imageHeight = FreeImage_GetHeight(bitmap32);

    GLubyte* textureData = FreeImage_GetBits(bitmap32);
    loadFromData(imageWidth, imageHeight, textureData);

    FreeImage_Unload(bitmap);

    if (bitsPerPixel != 32)
    {
        FreeImage_Unload(bitmap32);
    }

    return true;
}

bool Texture2D::loadFromData(int width, int height, const GLubyte* data)
{
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        width,
        height,
        0,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        data
    );

    return true;
}


void Texture2D::resetTexture()
{
	resetId();
}