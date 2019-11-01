#ifndef SIMPLELINEARALLOCATOR_H
#define SIMPLELINEARALLOCATOR_H

#include <vector>
#include <exception>
#include <type_traits>

namespace misc
{
	namespace mem
	{
		template<class Type, class Token = size_t, template<class> class Storage = std::vector>
		class SimpleLinearAllocator
		{
		public:
			using TStorage = Storage<Type>;

			using TToken = Token;

			using TType = Type;


			static const TToken EMPTY_TOKEN = static_cast<TToken>(-1);

			static const size_t INITIALL_CAPACITY = 1 << 6;


		public:
			SimpleLinearAllocator(size_t capacity = INITIALL_CAPACITY)
				: m_storage()
			{
				m_storage.reserve(capacity);
				m_storage.clear();
			}

			SimpleLinearAllocator(const SimpleLinearAllocator&) = default;
			SimpleLinearAllocator(SimpleLinearAllocator&&)      = default;

			~SimpleLinearAllocator() = default;

			SimpleLinearAllocator& operator = (const SimpleLinearAllocator&) = default;
			SimpleLinearAllocator& operator = (SimpleLinearAllocator&&)      = default;


		public:
			Type& get(const TToken& token)
			{
				//TODO: for debug only
				if (tokenValid(token))
				{
					return m_storage[token];
				}
				else
				{
					throw std::runtime_error("Token is invalid");
				}
			}

			const Type& get(const TToken& token) const
			{
				//TODO: for debug only
				if (tokenValid(token))
				{
					return m_storage[token];
				}
				else
				{
					throw std::runtime_error("Token is invalid");
				}
			}

			template<class ... Params>
			Token alloc(Params&& ... params)
			{
				//TODO: for debug only
				if (m_storage.size() <= m_storage.capacity())
				{
					m_storage.emplace_back(std::forward<Params>(params)...);
				}
				else
				{
					throw std::runtime_error("Storage is out of free space");
				}

				return m_storage.size() - 1;
			}

			void dealloc(const TToken& token)
			{
				//do nothing
			}

			void clear()
			{
				m_storage.clear();
			}


		public:
			bool tokenValid(const TToken& token) const
			{
				return (token <= m_storage.size()) 
					&& (token >= 0) 
					&& (token != EMPTY_TOKEN);
			}

			const TStorage& storage() const
			{
				return m_storage;
			}


		private:
			TStorage m_storage;
		};
	}
}

#endif