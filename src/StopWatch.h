#include <iostream>
#include <chrono>
#include <string_view>
template<typename OUTTYPE=float>
struct StopWatch {
	using timepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
	timepoint now;
	timepoint later;
	StopWatch() 
		: 
		now(std::chrono::high_resolution_clock::now()),
		later(now)
	{
	}
	OUTTYPE printDelta(std::string_view txt)
	{
		later = std::chrono::high_resolution_clock::now();
		std::chrono::duration<OUTTYPE, std::milli> fp_ms = later - now;
		std::cout << txt << "\t" << fp_ms.count() << " ms\n";
		now = std::chrono::high_resolution_clock::now();
		return fp_ms.count();
	};
};