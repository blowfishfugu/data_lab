#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2022.h"

int main(int argc, char** argv)
{
	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0]);//Set

	StopWatch clk;
#if FOO
#endif
	aoc2022_01();
	clk.printDelta("Day01 nosort (sums per group)");
	std::cout << "\n\n";
	
	aoc2022_02();
	clk.printDelta("Day02 rock->paper->scissors");
	std::cout << "\n\n";
	
	aoc2022_03();
	clk.printDelta("Day03 examine bags");
	std::cout << "\n\n";
	
	aoc2022_04();
	clk.printDelta("Day04 overlaps, bb-hittests\n");
	std::cout << "\n\n";
	
	return 0;
}