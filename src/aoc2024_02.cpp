#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include "SplitIterator.h"
#include <functional>
#include <iostream>
#include <cassert>
#include <execution>

void aoc2024_02()
{
	fs::path input(DataDir() / "2024_02.txt");
	TxtFile txt{ input };

	using I = std::int64_t;
	using VI = std::vector<I>;
	using Report = std::tuple<VI, I>;
	using Reports = std::vector<Report>;
	
	Reports reports{};
	I count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		VI data;
		SpaceIterator it{ line }; SpaceIterator end{};
		while (it != end)
		{
			data.emplace_back(toInt<I>(*it));
			++it;
		}
		reports.emplace_back(Report{ data, -1LL });
		++count;
	}
	std::cout << std::format("count: {}\n", count);
	
	auto checkReport = [](Report& report) {
		VI& data = std::get<0>(report);
		if (data.size() < 2) { return; }

		constexpr const I lower = 1;
		constexpr const I upper = 3;
		std::function<bool(const I l, const I r)> doesFit;
		I d1 = data[0];
		I d2 = data[1];
		if (d2 > d1) {
			doesFit = [](const I l, const I r) {
				I delta = r - l;
				return (delta >= lower && delta <= upper);
			};
		}
		else if (d2 < d1) {
			doesFit = [](const I l, const I r) {
				I delta = l - r;
				return (delta >= lower && delta <= upper);
			};
		}
		else {
			doesFit = [](const I, const I) {return false; };
		}

		for (size_t i = 1; i < data.size(); ++i) {
			I l = data[i - 1];
			I r = data[i];
		
			if (!doesFit(l, r)) {
				std::get<1>(report) = i;
				break;
			}
		}
	};

	std::for_each( std::execution::seq,
		reports.begin(), reports.end(), checkReport
	);

	auto itUnsafe=std::partition(reports.begin(), reports.end(), [](const Report& r) {
		return std::get<1>(r) == -1;
		});
	std::cout << std::format("safe: {}\n", std::distance(reports.begin(),itUnsafe));

	auto checkDampened = [checkReport]( Report& source ) {
		for (I dampen = 0LL; dampen < std::get<0>(source).size(); ++dampen)
		{
			Report damp1{ std::get<0>(source), -1LL };
			VI& data1 = std::get<0>(damp1);

			data1.erase(data1.begin() + dampen);
			checkReport(damp1);
			if (std::get<1>(damp1) == -1LL )
			{
				std::get<1>(source) = -1LL;
				return;
			}
		}
	};

	std::for_each( std::execution::par,
		itUnsafe, reports.end(), checkDampened
	);

	auto fullCount = std::count_if(reports.begin(), reports.end(), [](const Report& r) {
		return std::get<1>(r) == -1LL;
		});
	std::cout << std::format("safedampened: {}\n", fullCount);
}

