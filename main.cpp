#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2022.h"

int main(int argc, char** argv)
{
	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0]);//Set
	StopWatch overall;
	StopWatch clk;
#if FOO
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
	clk.printDelta("Day04 overlaps, bb-hittests");
	std::cout << "\n\n";
	
	aoc2022_05();
	clk.printDelta("Day05 The Binford CrateMaster9001");
	std::cout << "\n\n";

	aoc2022_06();
	clk.printDelta("Day06 tuning trouble (unique packets)");
	std::cout << "\n\n";

	aoc2022_06_withInts();
	clk.printDelta("Day06 popcnt tuning trouble (unique packets)");
	std::cout << "\n\n";

	aoc2022_07();
	clk.printDelta("Day07 not enough diskspace");
	std::cout << "\n\n";

	aoc2022_08();
	clk.printDelta("Day08 counting trees");
	std::cout << "\n\n";

	aoc2022_08_cleanup();
	clk.printDelta("Day08 counting trees");
	std::cout << "\n\n";

	aoc2022_09();
	clk.printDelta("Day09 segmented ropes");
	std::cout << "\n\n";

	aoc2022_10();
	clk.printDelta("Day10 cathode ray");
	std::cout << "\n\n";

	aoc2022_11();
	clk.printDelta("Day11 monkey business, refresher modulo");
	std::cout << "\n\n";

	aoc2022_12();
	clk.printDelta("Day12 Hill Climbing Djkstra");
	std::cout << "\n\n";

	aoc2022_13();
	clk.printDelta("Day13 distress signal, sorting packets");
	std::cout << "\n\n";

	//ab hier gehts auf performance, nur in release-build aktivieren
#ifdef NDEBUG
	//dieses stück braucht (137*137)/2 if's pro sandkorn
	aoc2022_14();
	clk.printDelta( "Day14 regolith reservoir" );
	std::cout << "\n\n";

	aoc2022_15();
	clk.printDelta("Day15 beacon exclusion zone");
	std::cout << "\n\n";

#endif
#endif
	aoc2022_16();
	clk.printDelta("Day16");
	std::cout << "\n\n";

	aoc2022_17();
	clk.printDelta("Day17 Pyroclastic Flow");
	std::cout << "\n\n";


	overall.printDelta("\noverall");
	return 0;
}