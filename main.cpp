#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2022.h"

int main(int argc, char** argv)
{
	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0]);//Set

	StopWatch clk;
	aoc2022_01();
	clk.printDelta("Day01 nosort");
	aoc2022_02();
	clk.printDelta("Day02");
	return 0;
}