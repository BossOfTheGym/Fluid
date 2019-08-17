#ifndef PING_PONG_BUFFER_H
#define PING_PONG_BUFFER_H


//FR = FramebufferResource
template<class FR>
class PingPongBuffer
{
public:
	using FRT = std::remove_cv_t<std::remove_reference_t<FR>>;

	static const int BUFFERS = 2;


public:
	PingPongBuffer(FR&& firstResource, FR&& secondResource)
		: m_previousBufferIndex(0)
		, m_currentBufferIndex(1)
		, m_framebufferResources{std::move(firstResource), std::move(secondResource)}
	{}


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

#endif