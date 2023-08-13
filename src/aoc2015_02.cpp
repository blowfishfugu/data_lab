#include "DataDir.h"
#include <iostream>
#include <format>
#include <vector>
#include <cassert>
#include <string_view>
#include <algorithm>
#include <functional>
#include <numeric>
#include <execution>

void aoc2015_02()
{
	fs::path input(DataDir() / "2015_02.txt");
	
	struct box {
		__int64 l{};
		__int64 w{};
		__int64 h{};
		__int64 area{};
		__int64 smallestFace{};
		__int64 wrapping{};

		__int64 volume{};
		__int64 smallestRibbon{};
		__int64 ribbonLen{};

		box(const std::string_view& dims)
		{
			size_t x1=dims.find('x', 0);
			if( x1!=std::string_view::npos)
			{ 
				size_t x2 = dims.find('x', x1 + 1);
				if (x2 != std::string_view::npos)
				{
					std::from_chars(dims.data(), dims.data()+x1, l);
					std::from_chars(dims.data()+x1+1, dims.data()+x2, w);
					std::from_chars(dims.data() + x2+1, dims.data() + dims.length(), h);
				}
			}
		}
	};
	std::vector<box> boxes;
	
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}
		boxes.emplace_back(line);
	}
	inFile.close();
	
	std::for_each( std::execution::par,
		boxes.begin(), boxes.end(), [](box& b){
			__int64 lw = b.l * b.w;
			__int64 wh = b.w * b.h;
			__int64 hl = b.h * b.l;
			b.area = (lw + wh + hl) + (lw + wh + hl);
			b.smallestFace = std::min(std::min(lw, wh), hl);
			b.wrapping = b.area + b.smallestFace;

			b.volume = lw * b.h;

			__int64 r1 = b.l + b.l + b.w + b.w;
			__int64 r2 = b.l + b.l + b.h + b.h;
			__int64 r3 = b.w + b.w + b.h + b.h;
			b.smallestRibbon= std::min(std::min(r1, r2), r3);

			b.ribbonLen = b.volume + b.smallestRibbon;
		});

	__int64 wrapping = 0LL;
	__int64 ribbon = 0LL;
	for (const box& b : boxes) { 
		wrapping += b.wrapping; ribbon += b.ribbonLen; 
	}
	std::cout << std::format("{} boxes, {} wrapping, {} ribbon\n", boxes.size(), wrapping, ribbon);
	assert(wrapping == 1598415);
	assert(ribbon == 3812909);
	
}