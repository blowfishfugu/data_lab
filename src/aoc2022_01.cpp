#include "DataDir.h"
#include <iostream>

void aoc2022_01()
{
	fs::path input(DataDir() / "2022_01.txt");
	
	std::ifstream inFile(input);
	__int64 maxSum[3]{ 0,0,0 };
	__int64 curSum = 0;
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			if (curSum > maxSum[0])
			{
				maxSum[2] = maxSum[1];
				maxSum[1] = maxSum[0];
				maxSum[0] = curSum;
			}
			else if (curSum > maxSum[1])
			{
				maxSum[2] = maxSum[1];
				maxSum[1] = curSum;
			}
			else if (curSum > maxSum[2])
			{
				maxSum[2] = curSum;
			}
			curSum = 0;
			continue;
		}

		curSum += std::stoll(line);
	}

	std::cout << maxSum[0] << "\n";
	std::cout << maxSum[0] + maxSum[1] + maxSum[2] << "\n";
}