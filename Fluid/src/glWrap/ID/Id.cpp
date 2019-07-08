#include "Id.h"


//constructors
Id::Id() : m_id(Id::Empty)
{}

Id::Id(GLuint id) : m_id(id)
{}

Id::Id(Id&& id) : m_id(id.id())
{
	id.resetId();
}

//operators
Id& Id::operator = (Id&& id)
{
	std::swap(m_id, id.m_id);

	id.resetId();
}

//core
GLuint Id::id() const
{
	return m_id;
}

bool Id::valid() const
{
	return m_id != Id::Empty;
}

