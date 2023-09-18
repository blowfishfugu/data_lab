#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2015.h"

#include <string_view>
#include <string>
#include <stack>
std::string reverseWords(const std::string_view& s)
{
	std::ostringstream os;
	using Stack = std::stack<char>;
	Stack word;
	auto popStack = [](Stack& word, std::ostream& os) {
		while (!word.empty())
		{
			os << word.top();
			word.pop();
		}
	};
	for (const char c : s) {
		if (c == ' ') {
			popStack(word,os);
			os << ' ';
		}
		else {
			word.push(c);
		}
	}
	popStack(word,os);
	return os.str();
}

template<ptrdiff_t cap=5>
void reverse_cap(std::string::iterator wordBegin, std::string::iterator wordEnd)
{
	if (std::distance(wordBegin, wordEnd) >= cap)
	{
		std::reverse(wordBegin, wordEnd);
	}
}

std::string spinWords(std::string str)
{
	constexpr ptrdiff_t cap = 5;
	auto wordBegin = str.begin();
	for (auto pos = str.begin(); pos != str.end(); ++pos)
	{
		if (*pos == ' ') {
			reverse_cap<5>(wordBegin, pos);
			wordBegin = pos + 1;
		}
	}
	reverse_cap<5>(wordBegin, str.end());
	return str;
}

#include <map>

std::vector<int> deleteNth(std::vector<int> arr, int n)
{
	if (n == 0) {
		return {};
	}
	std::map<int, int> hist;
	std::vector<int> output;
	for (const int num : arr)
	{
		int& count = hist[num];
		if (count < n) {
			output.emplace_back(num);
		}
		++count;
	}
	return output;
}

#include <numeric>
std::string get_middle(std::string input)
{
	std::string result;
	size_t len = input.length();
	if (len <= 2) { return input; }
	
	size_t mid = len / 2;
	if ((len & 0x01) == 0x00)
	{
		result += input[mid-1];
	}
	result += input[mid];
	
	return result;
}


int main(int argc, char** argv)
{
#ifdef UNIT_TEST
	extern void IteratorCheck();
	IteratorCheck();
#endif

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2015");//Set

	StopWatch clk;

	//aoc2015_01();clk.printDelta("Day01");std::cout << "\n\n";
	//aoc2015_02();clk.printDelta("Day02");std::cout << "\n\n";
	//aoc2015_03();clk.printDelta("Day03");std::cout << "\n\n";
	//aoc2015_04();clk.printDelta("Day04");std::cout << "\n\n";
	//aoc2015_05();clk.printDelta("Day05");std::cout << "\n\n";
	//aoc2015_06();clk.printDelta("Day06");std::cout << "\n\n";
	//aoc2015_07();clk.printDelta("Day07");std::cout << "\n\n";
	std::cout << get_middle("test") << "\n"; // ==> "sihT si na !elpmaxe"
	std::cout << get_middle("testing") << "\n"; // ==> "sihT si na !elpmaxe"
	std::cout << get_middle("double  spaces") << "\n";// ==> "elbuod  secaps"")
	return 0;
}