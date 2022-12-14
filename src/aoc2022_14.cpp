#include "DataDir.h"
#include <iostream>
#include <map>
#include <array>
#include <algorithm>
#include <functional>

#include <ppl.h>
#include <concrt.h>

void aoc2022_14()
{
	fs::path input(DataDir() / "2022_14.txt");
	std::ifstream inFile(input);
	
	
	for( std::string line; std::getline( inFile, line );)
	{
		if( line.length() == 0 ) { break; }
	}
	

	
}