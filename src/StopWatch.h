#include <iostream>
#include <chrono>

template<typename OUTTYPE=float>
struct StopWatch {
	using timepoint = std::chrono::time_point<std::chrono::steady_clock>;
	timepoint now;
	timepoint later;
	StopWatch() : now(std::chrono::high_resolution_clock::now()),later(now)
	{

	}
	OUTTYPE printDelta(const char* txt)
	{
		later = std::chrono::high_resolution_clock::now();
		std::chrono::duration<OUTTYPE, std::milli> fp_ms = later - now;
		std::cout << txt << fp_ms.count() << " ms\n";
		now = std::chrono::high_resolution_clock::now();
		return fp_ms.count();
	};
};