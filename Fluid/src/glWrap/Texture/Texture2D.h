#pragma once

#include <Common.h>

#include <glWrap/OpenGL.h>
#include <glWrap/ID/Id.h>


class Texture2D : public Id
{
public:
    static void active(GLenum texture);


public:
	Texture2D();

    Texture2D(const String& location);

    Texture2D(int width, int height, const std::vector<GLubyte>& data);

    Texture2D(int width, int height, const GLubyte* data);


    Texture2D(const Texture2D& tex) = delete;

    Texture2D(Texture2D&& tex);


    ~Texture2D();


    Texture2D& operator = (const Texture2D& tex) = delete;

    Texture2D& operator = (Texture2D&& tex);


public:
    void bind() const;

	void unbind() const;


    void texParameteri(GLenum name, GLint parameter) const;


    void deleteTexture();


private:
    bool loadFromLocation(const String& location);

    bool loadFromData(int width, int height, const GLubyte* data);


    void resetTexture();
};