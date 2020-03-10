#pragma once

#include <utility>

namespace misc
{
// in fact this class is useless as shit counting that fact 
// that Framebuffer can be swaped with another buffer perfectly

// FR = FramebufferResource
	template<class FR>
	class PingPongBuffer
	{
	public:
		using FRT = std::remove_cv_t<std::remove_reference_t<FR>>;

		static const int BUFFERS = 2;


	public:
		PingPongBuffer(FR firstResource, FR secondResource)
			: m_previousBufferIndex(0)
			, m_currentBufferIndex(1)
			, m_framebufferResources{std::forward<FR>(firstResource), std::forward<FR>(secondResource)}
		{
			// m_framebufferResources[0] = std::move(firstResource);
			// m_framebufferResources[1] = std::move(secondResource);
		}

		PingPongBuffer(const PingPongBuffer&) = delete;

		PingPongBuffer(PingPongBuffer&& buffer)
			: m_previousBufferIndex(buffer.m_previousBufferIndex)
			, m_currentBufferIndex(buffer.m_currentBufferIndex)
		{
			m_framebufferResources[0] = std::move(buffer.m_framebufferResources[0]);
			m_framebufferResources[1] = std::move(buffer.m_framebufferResources[1]);
		}

		~PingPongBuffer() = default;

		PingPongBuffer& operator = (const PingPongBuffer&) = delete;

		PingPongBuffer& operator = (PingPongBuffer&& buffer)
		{
			std::swap(m_currentBufferIndex, buffer.m_currentBufferIndex);
			std::swap(m_previousBufferIndex, buffer.m_previousBufferIndex);
			std::swap(m_framebufferResources, buffer.m_framebufferResources);

			return *this;
		}

	public:
		void swap()
		{
			m_previousBufferIndex = m_currentBufferIndex;
			m_currentBufferIndex = (m_currentBufferIndex + 1) % BUFFERS;
		}


		FRT& current()
		{
			return m_framebufferResources[m_currentBufferIndex];
		}

		FRT& previous()
		{
			return m_framebufferResources[m_previousBufferIndex];
		}


	private:
		FRT m_framebufferResources[BUFFERS];

		int m_currentBufferIndex;
		int m_previousBufferIndex;
	};
}