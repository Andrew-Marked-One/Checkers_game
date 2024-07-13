#pragma once
#include <chrono>


class Timer {
	using Clock = std::chrono::steady_clock;
	using TimePoint = Clock::time_point;
	using Milliseconds = std::chrono::milliseconds;

public:
	Timer();
	Timer(int msLimit);

	void start();
	bool timeRanOut() const;
	int timeElapsed() const;
	void setTimeLimit(int msLimit);
	void pause();

private:
	TimePoint m_startTime;
	TimePoint m_pauseTime;
	Milliseconds m_timeLimit = Milliseconds(0);
	bool m_isPaused = false;
};
