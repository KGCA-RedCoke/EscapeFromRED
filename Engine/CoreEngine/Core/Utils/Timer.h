#pragma once

#include <chrono>

using namespace std::chrono;

class Timer
{
public:
	Timer()
	{
		Reset();
	}

	void Reset()
	{
		m_Start = high_resolution_clock::now();
	}

	float Elapsed()
	{
		return duration_cast<nanoseconds>(high_resolution_clock::now() - m_Start).
		count() * 0.001f * 0.001f * 0.001f;
	}

	float ElapsedMillis()
	{
		return Elapsed() * 1000.0f;
	}

private:
	time_point<high_resolution_clock> m_Start;
};
