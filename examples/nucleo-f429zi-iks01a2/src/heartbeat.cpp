
#include "heartbeat.hpp"

Heartbeat::Heartbeat(const uint32_t period)
	: m_period(period)
{
}

void Heartbeat::update(const uint32_t timestamp)
{
	// Note: very basic implementaion, not taking overflows into account here
	const uint32_t diff = timestamp - m_previous;
	m_elapsed = (diff >= m_period);
	if (m_elapsed) {
		m_previous = timestamp;
	}
}
