#pragma once
#include <string_view>
#include <vector>
#include <concepts>
#include <tuple>
#include <charconv>
#include "SplitIterator.h"

template<std::integral Int>
Int toInt(const std::string_view& item)
{
	Int num{};
	std::from_chars(item.data(), item.data() + item.size(), num);
	return num;
}

template<char c>
void trim(std::string_view& line)
{
	while (	line.starts_with(c) ) { line.remove_prefix(1); }
	while ( line.ends_with(c) ){ line.remove_suffix(1); }
}

template<char c, bool withTrim = false>
std::tuple<std::string_view, std::string_view> pairSplit(std::string_view line)
{
	if constexpr (withTrim)
	{
		trim<' '>(line);
	}

	SplitIterator<c> it{ line }; SplitIterator<c> itEnd{};
	if (it != itEnd) {
		std::string_view first{ *it };
		++it;
		if (it != itEnd) {
			std::string_view second{ *it };
			return { first,second };
		}
		else {
			return { first,"" };
		}
	}
	return {};
}

template<char c, std::integral Int, bool withTrim = true>
std::vector<Int> intSplit(std::string_view line)
{
	if constexpr (withTrim)
	{
		trim<' '>(line);
	}

	std::vector<Int> result;
	SplitIterator<c> it{ line }; SplitIterator<c> end{};
	while (it != end)
	{
		std::string_view item = *it;
		if (item.size() > 0)
		{
			Int num = toInt<Int>(item);
			result.emplace_back(num);
		}
		++it;
	}
	return result;
}