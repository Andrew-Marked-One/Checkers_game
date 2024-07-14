#include "Timer.h"


Timer::Timer() {}

Timer::Timer(int msLimit)
	: m_timeLimit(msLimit) {}

void Timer::start() {
	m_startTime = Clock::now();
}

bool Timer::timeRanOut() const {
	if (m_timeLimit == Milliseconds(0)) {
		return false;
	}

	if (m_isPaused) {
		Milliseconds duration = std::chrono::duration_cast<Milliseconds>(m_pauseTime - m_startTime);
		return duration > m_timeLimit;
	}
	else {
		Milliseconds duration = std::chrono::duration_cast<Milliseconds>(Clock::now() - m_startTime);
		return duration > m_timeLimit;
	}
}

int Timer::timeElapsed() const {
	if (m_isPaused) {
		return static_cast<int>(std::chrono::duration_cast<Milliseconds>(m_pauseTime - m_startTime).count());
	}
	else {
		return static_cast<int>(std::chrono::duration_cast<Milliseconds>(Clock::now() - m_startTime).count());
	}
}

void Timer::setTimeLimit(int msLimit) {
	m_timeLimit = Milliseconds(msLimit);
}

void Timer::pause() {
	if (m_isPaused) {
		Milliseconds duration = std::chrono::duration_cast<Milliseconds>(Clock::now() - m_pauseTime);
		m_startTime += duration;
	}
	else {
		m_pauseTime = Clock::now();
	}

	m_isPaused = !m_isPaused;
}
