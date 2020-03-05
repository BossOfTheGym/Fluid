#pragma once

#include <glWrap/OpenGL.h>

namespace gl
{
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
		Id(Id&& id) noexcept;

		Id& operator = (const Id&) = delete;
		Id& operator = (Id&& id) noexcept;

		~Id() = default;


	public:
		GLuint id() const;

		bool valid() const;

		void resetId();


	private:
		GLuint m_id;
	};
}
