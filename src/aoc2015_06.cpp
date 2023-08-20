#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"

#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>
#include <regex>
#include "gdi_window.h"

void aoc2015_06()
{
	std::regex perLine(R"regex((turn on |turn off |toggle )(\d*),(\d*) through (\d*),(\d*))regex", std::regex_constants::optimize);
	TxtFile txt{ DataDir() / "2015_06.txt" };
	AsyncQueue<RectData> inputs;
	int width = 1000;
	int height = 1000;
	gdi_window EyeCandy(inputs,width,height);
	EyeCandy.start_window();



	for (const auto& line : txt)
	{
		std::string strLine(line);
		std::smatch items;
		if (std::regex_match(strLine, items, perLine))
		{
			if (items.size() > 5)
			{
				std::string cmd{ items[1].str() };
				coord start{};
				std::string s1(items[2].str());
				std::string s2(items[3].str());
				std::from_chars(s1.data(), s1.data() + s1.length(), std::get<0>(start));
				std::from_chars(s2.data(), s2.data() + s2.length(), std::get<1>(start));

				coord end{};
				std::string e1(items[4].str());
				std::string e2(items[5].str());
				std::from_chars(e1.data(), e1.data() + e1.length(), std::get<0>(end));
				std::from_chars(e2.data(), e2.data() + e2.length(), std::get<1>(end));

				RectData data{ nullptr,start,end };
				if (cmd == "turn off ") { 
					//data.command = [](Pixel& p) { p.color = 0; }; 
					data.command = [](Pixel& p) { p.brightness -= 1; if (p.brightness < 0) { p.brightness = 0; } p.color = p.brightness; };
				}
				else if (cmd == "turn on ") { 
					//data.command = [](Pixel& p) { p.color = RGB(255,255,255); };
					data.command = [](Pixel& p) { p.brightness += 1; p.color = p.brightness; };
				}
				else if (cmd == "toggle ") {
					//data.command = [](Pixel& p) {
					//	if (p.color > 0) { p.color = RGB(0, 0, 0); }
					//	else { p.color = RGB(255, 255, 255); }
					//};
					data.command = [](Pixel& p) { p.brightness += 2; p.color = p.brightness; };
				}

				auto& [sx, sy] = data.topLeft;
				auto& [ex, ey] = data.bottomRight;
					for (int y = sy; y <= ey; ++y)
					{
						for (int x = sx; x <= ex; ++x) {
							size_t pos = y * width + x;
							data.command( EyeCandy.pixels[pos] );
						}
					}
				inputs.push(data);
				//EyeCandy.trigger_update();
			}
		}
	}

	EyeCandy.trigger_update();
	EyeCandy.wait_for_close();

	int hits = 0;
	__int64 brightness = 0;
	for (const Pixel& p : EyeCandy.pixels)
	{
		if (p.color > 0)
		{
			++hits;
		}
		brightness += p.brightness;
	}
	std::cout << hits << " " << brightness << "\n"; //377891 //14110788
}