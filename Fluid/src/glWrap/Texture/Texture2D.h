#pragma once

#include <Common.h>

#include <glWrap/Consts.h>


class Texture2D
{
public:
    static void active(GLenum texture);

    static void unbind();


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


    void bind() const;

    void texParameteri(GLenum name, GLint parameter) const;


    GLuint getId() const;


    void deleteTexture();


private:
    int loadFromLocation(const String& location);

    int loadFromData(int width, int height, const GLubyte* data);


    void resetTexture();


private:
    GLuint mId;
};