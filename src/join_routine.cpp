#include "DataDir.h"
#include "TxtFile.h"
#include "TransparentHash.h"
#include "SplitIterator.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <tuple>

using OrderMap = std::unordered_map<std::string_view, size_t, string_hash, std::equal_to<> >;
using Row = std::vector<std::string_view>;
using Table = std::vector<Row>;
using IndexedTable = std::tuple<Table, OrderMap>;
//returns, complete table and Map OrderID to RowNumber
IndexedTable createOrderIndex(const TxtFile& paypal, size_t orderColumn) {
	OrderMap id_to_index;
	Table table;
	bool isHead = true;
	for (size_t lineIndex = 0; const auto & line : paypal)
	{
		if (isHead) { isHead = false; continue; } //skip Headline?

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
		if (row.size() > 0)
		{
			table.emplace_back(row);
			++lineIndex;
		}
	}
	return { table,id_to_index };
}

void outrow(std::ostream& os, const Row& row)
{
	bool first = true;
	for (const auto& sv : row)
	{
		if (first) { first = false; }
		else { os << ','; }
		os << sv.substr(0, sv.size());
	}
}

void printImsAndCompletedBills( std::ostream& os, const IndexedTable& fromIms, const IndexedTable& fromPaypal, size_t keyIndex)
{
	const auto& [ims, indexIms] = fromIms;
	const auto& [pal, indexPal] = fromPaypal;
	for (const Row& im : ims)
	{
		//complete row of ims
		outrow(os, im);
		//+ bill, 0 account if available
		if (im.size() <= keyIndex) { continue; }

		if (auto found = indexPal.find(im[keyIndex]);
			found != indexPal.end()
			)
		{
			const size_t palIndex = found->second;
			const Row& palRow = pal[palIndex];
			if (palRow.size() > 5)
			{
				os << "\n" << palRow[5] << "," << palRow[0];
			}
		}
		os << '\n';
	}
}

void join_routine(std::string_view imsFileName, std::string_view paypalFileName)
{
	fs::path inIMS(DataDir() / imsFileName);
	fs::path inPaypal(DataDir() / paypalFileName);
	
	TxtFile paypal(inPaypal);
	TxtFile ims(inIMS);

	const IndexedTable fromPaypal=createOrderIndex(paypal, 2);
	const IndexedTable fromIms=createOrderIndex(ims, 2);
	
	//fs::path acc_import(DataDir() / "acc_import.csv");
	//std::ofstream outfile(acc_import);
	//printImsAndCompletedBills(outfile, fromIms, fromPaypal, 2);
	printImsAndCompletedBills(std::cout, fromIms, fromPaypal, 2);
}
