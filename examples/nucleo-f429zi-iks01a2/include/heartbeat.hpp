#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <cinttypes>

class Heartbeat
{
public:
	explicit Heartbeat(uint32_t period);

	void update(uint32_t timestamp);
	[[nodiscard]] bool periodElapsed() const
	{
		return m_elapsed;
	}

private:
	const uint32_t m_period = 0;
	uint32_t m_previous = 0;
	bool m_elapsed = false;
};

#endif
