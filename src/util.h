#pragma once

#include <random>
#include <cmath>

inline int random_int(int from, int to)
{
	std::random_device rd;  // Seed generator
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_int_distribution<> dis(from, to);
	return dis(gen);
}

inline float random_float(float from, float to)
{
	std::random_device rd;  // Seed generator
	std::mt19937 gen(rd()); // Mersenne Twister engine
	std::uniform_real_distribution<float> dis(from, to);
	return dis(gen);
}

class FPSCounter
{
private:
	float last_time;
	int frame_count;
	float fps;
	float update_interval_secs;

public:
	FPSCounter() : last_time(glfwGetTime()), frame_count(0), fps(0.0f), update_interval_secs(0.5f) {};

	float Update()
	{
		float current_time = glfwGetTime();
		frame_count++;

		if (current_time - last_time >= update_interval_secs) {
			fps = frame_count / (current_time - last_time);
			last_time = current_time;
			frame_count = 0;
		}

		return fps;
	}
};