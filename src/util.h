#pragma once

#include <random>
#include <cmath>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <format>

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

// Maximize thread usage without using every single thread to avoid OS lock
inline unsigned int GetOptimalThreadCount() {
	unsigned int max_threads = std::thread::hardware_concurrency();
	// Ensure at least 1 thread is returned in case hardware_concurrency() is 0
	return (max_threads > 1) ? max_threads - 1 : 1;
}

inline void sleep_ms(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

inline void print(std::string text)
{
	std::cout << text << std::endl;
}

template<typename T, typename Predicate>
inline std::vector<T> filter_vector(const std::vector<T>& input, Predicate predicate)
{
	std::vector<T> filtered;
	for (const auto& item : input) {
		if (predicate(item))
			filtered.push_back(item);
	}
	return filtered;
}


class FPSCounter
{
private:
	std::chrono::high_resolution_clock::time_point last_time;
	int frame_count;
	float fps;
	float update_interval_secs;

public:
	FPSCounter() : last_time(std::chrono::high_resolution_clock::now()), frame_count(0), fps(0.0f), update_interval_secs(0.5f) {};

	float Update()
	{
		auto current_time = std::chrono::high_resolution_clock::now();
		frame_count++;

		std::chrono::duration<double> elapsed = current_time - last_time;

		if (elapsed.count() >= update_interval_secs) {
			fps = frame_count / elapsed.count();
			last_time = current_time;
			frame_count = 0;
		}

		return fps;
	}
};

class Timer
{
public:
	std::string name;
	std::chrono::high_resolution_clock::time_point start_time;
	std::chrono::high_resolution_clock::time_point last_split;
	std::chrono::high_resolution_clock::time_point end_time;

	std::vector<std::chrono::duration<double>> splits;

	Timer(std::string name) : name(name)
	{
		Reset();
	};

	void Reset(std::string rename = "")
	{
		if (rename.size()) {
			Stop();
			name = rename;
		}
		start_time = std::chrono::high_resolution_clock::now();
		last_split = std::chrono::high_resolution_clock::now();
		end_time = std::chrono::high_resolution_clock::now();
		splits.clear();
	};

	void LogSplit(bool display = true)
	{
		auto curr = std::chrono::high_resolution_clock::now();
		splits.push_back(
			curr - last_split
		);
		last_split = curr;

		if (display) {
			std::cout << std::format(
				"{0} timer split: {1}", name, splits[splits.size() - 1]
			) << std::endl;
		}
	};

	void Stop(bool display = true, bool display_splits = false)
	{
		end_time = std::chrono::high_resolution_clock::now();
		splits.push_back(
			end_time - last_split
		);
		std::chrono::duration<double> total_duration = end_time - start_time;

		if (display) {
			std::cout << std::format(
				"{0} timer total duration: {1}", name, total_duration
			) << std::endl;

			if (display_splits) {
				std::cout << std::format(
					"{0} timer average split: {1}", name, total_duration / splits.size()
				) << std::endl;
			}
		}
	};
};