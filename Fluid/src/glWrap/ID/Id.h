#ifndef ID_h
#define ID_h

#include <Common.h>

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
	Id(Id&& id) noexcept(true);

	Id& operator = (const Id&) = delete;
	Id& operator = (Id&& id) noexcept(true);

	~Id() = default;

	
public:
	GLuint id() const;

	bool valid() const;

	void resetId();


private:
	GLuint m_id;
};


#endif