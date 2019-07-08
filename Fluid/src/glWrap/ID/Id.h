#ifndef ID_h
#define ID_h

#include <glWrap/OpenGL.h>


class Id
{
public:
	enum : GLuint
	{
		Empty = 0U
	};

public:
	Id();

	Id(GLuint id);

	Id(const Id&) = delete;
	Id(Id&& id);

	Id& operator = (const Id&) = delete;
	Id& operator = (Id&& id);

	~Id() = default;


public:
	GLuint id() const;

	bool valid() const;


private:
	void resetId();


protected:
	GLuint m_id;
};


#endif