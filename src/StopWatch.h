#include <iostream>
#include <chrono>
#include <string_view>
template<typename OUTTYPE=float>
struct StopWatch {
	using clocktype = std::chrono::high_resolution_clock;
	using timepoint = std::chrono::time_point<clocktype>;
	timepoint now;
	timepoint later;
	StopWatch() : now(clocktype::now()),later(now)
	{

	}
	OUTTYPE printDelta(std::string_view txt)
	{
		later = clocktype::now();
		std::chrono::duration<OUTTYPE, std::milli> fp_ms = later - now;
		std::cout << txt << "\t" << fp_ms.count() << " ms\n";
		now = clocktype::now();
		return fp_ms.count();
	};
};