#include "DataDir.h"
#include "TxtFile.h"
#include "TransparentHash.h"
#include "SplitIterator.h"
#include <iostream>
#include <format>
#include <unordered_map>
#include <tuple>
using OrderMap = std::unordered_map<std::string_view, size_t, string_hash, std::equal_to<> >;
using Row = std::vector<std::string_view>;
using Table = std::vector<Row>;

//returns, complete table and Map OrderID to RowNumber
std::tuple<Table,OrderMap> createOrderIndex(const TxtFile& paypal, size_t orderColumn) {
	OrderMap id_to_index;
	Table table;
	for (size_t lineIndex = 0; const auto & line : paypal)
	{
		Row row;
		SplitIterator<','> begin(line);
		SplitIterator<','> end;
		size_t itemIndex = 0;
		for (auto it = begin; it != end; ++it)
		{
			const std::string_view item = *it;
			if (itemIndex == orderColumn)
			{
				id_to_index[item] = lineIndex;
			}
			row.emplace_back(item);
			++itemIndex;
		}
		++lineIndex;
		if (row.size() > 0)
		{
			table.emplace_back(row);
		}
	}
	return { table,id_to_index };
}


void join_routine(std::string_view imsFileName, std::string_view paypalFileName)
{
	fs::path inIMS(DataDir() / imsFileName);
	fs::path inPaypal(DataDir() / paypalFileName);
	
	TxtFile ims(inIMS);
	TxtFile paypal(inPaypal);

	auto [paypalTable,ordersOfPaypal]=createOrderIndex(paypal, 2);
	auto [imsTable, ordersOfIms]=createOrderIndex(ims, 2);
	
}