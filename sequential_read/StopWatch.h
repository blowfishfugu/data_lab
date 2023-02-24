#pragma once
#include <chrono>
#include <string_view>
#include <iostream>
struct StopWatch {
	using timepoint = std::chrono::time_point<std::chrono::steady_clock>;
	using clock = std::chrono::high_resolution_clock;
	timepoint now;
	timepoint from_start;
	StopWatch() 
		: from_start(clock::now())
	{
		now = from_start;
		return;
	}

	void reset() noexcept{
		from_start = now = (clock::now());
	}

	float checkpoint(std::string_view txt) noexcept{
		const auto local_now = clock::now();
		std::chrono::duration<float, std::milli> fp_total = local_now - from_start;
		std::chrono::duration<float, std::milli> fp_ms = local_now - now;
		std::cout << txt.data() << fp_ms.count() << "ms\ttotal:" << fp_total.count() << "ms\n";
		now = clock::now(); //..ignore cout-time
		return fp_ms.count();
	};
};