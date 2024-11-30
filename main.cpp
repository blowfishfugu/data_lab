#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2023.h"

#include <set>
#include <bitset>
#include <functional>
#include <unordered_set>
#include <map>
#include <array>
constexpr const size_t maxCourses = 2000UL;
static int _id{};
class Solution {
    struct course {
        int id{};
        course(): id{_id++}{ }
        std::vector<std::reference_wrapper<course>> deps{};
        bool fulFill(std::vector<int>& order, std::bitset<maxCourses>& attended, std::bitset<maxCourses>& parents) const
        {
            if (attended.test(id)) { return false; }
            if (parents.test(id)) {
                return true;
            }
            parents.set(id);

            for (const course& dep : deps)
            {
                bool recursed = dep.fulFill(order, attended, parents);
                if (recursed)
                {
                    return true;
                }
            }
            parents.reset(id);

            order.push_back(id);
            attended.set(id);
            return false;
        }
    };

public:
    std::vector<int> findOrder(int numCourses, std::vector<std::vector<int>>& prerequisites) {
        _id = 0;
        if (numCourses <= 0) {
            return {};
        }

        std::vector<course> courses;
        courses.resize(numCourses);
        int _id = 0;
        for (course& c : courses) {
            c.id = _id;
            ++_id;
        }

        for (const auto& c_dep : prerequisites)
        {
            if (c_dep.size() < 2) { continue; }
            const int id = c_dep[0];
            const int dep = c_dep[1];
            courses[id].deps.push_back(std::ref<course>(courses[dep]));
        }

        std::sort(courses.begin(), courses.end(), [](course& l, course& r) {return l.deps.size() > l.deps.size(); });

        std::vector<int> orders;
        orders.reserve(numCourses);
        std::bitset<maxCourses> attended;
        std::bitset<maxCourses> parents;
        for ( const course& c : courses)
        {
            bool recursed = c.fulFill(orders, attended, parents);
            if (recursed)
            {
                return {};
            }
        }

        return orders;
    }
};

namespace trie
{
    constexpr const std::string_view test_case = R"raw(
["Trie","insert","search","search","startsWith","insert","search"]
        [[], ["apple"], ["apple"], ["app"], ["app"], ["app"], ["app"]])raw";

    static std::vector<std::string> commands{ "Trie","insert","search","search","startsWith","insert","search" };
    static std::vector<std::string> arguments{ "" ,"apple","apple","app","app","app","app" };

    class Trie final{
        struct CharNode {
            std::array<std::unique_ptr<CharNode>, 'z' - 'a' + 1> childs;
            bool wordComplete{ false };
        };

        CharNode data;
    public:
        Trie() {

        }

        void insert(std::string const& word) {
            CharNode* root = &data;
            for (size_t i=0;i<word.size();++i)
            {
                const char& c = word[i];
                int pos = c - 'a';
                if (!root->childs[pos]) {
                    root->childs[pos]= std::make_unique<CharNode>();
                }
                root = root->childs[pos].get();
            }
            if (root) { root->wordComplete = true; }
        }

        bool search(std::string const& word) const{
            const CharNode* root = &data;
            for (const char& c : word) {
                int pos = c - 'a';
                if (!root->childs[pos]) { 
                    return false; 
                }
                root = root->childs[pos].get();
            }
            return root->wordComplete;
        }

        bool startsWith(std::string const& prefix) const{
            const CharNode* root = &data;
            for (const char& c : prefix) {
                int pos = c - 'a';
                if (!root->childs[pos]) { 
                    return false; 
                }
                root = root->childs[pos].get();
            }
            return true; // root != &data; //allow-empty?
        }
    };

    /**
     * Your Trie object will be instantiated and called as such:
     * Trie* obj = new Trie();
     * obj->insert(word);
     * bool param_2 = obj->search(word);
     * bool param_3 = obj->startsWith(prefix);
     */

    auto zip = [](const std::vector<std::string>& l, const std::vector<std::string>& r)
    {
        std::vector<std::tuple<std::string, std::string>> result;
        auto itl = l.cbegin();
        auto itr = r.cbegin();
        while (itl != l.cend() && itr != r.cend())
        {
            result.emplace_back(*itl, *itr);
            ++itl;
            ++itr;
        }
        return result;
    };
}

void runTrie() {
    using namespace trie;
    using TrieType = Trie;
    static TrieType* current = nullptr;
    static std::vector<TrieType*> tries;
    std::vector<std::tuple<std::string, std::string>> tests = zip(commands, arguments);

    std::map<std::string, std::function<bool(TrieType*, std::string const&)> > cmds{
        { "Trie", [](TrieType*,std::string const&) { current = new TrieType(); tries.push_back(current); return true; } },
        { "insert", [](TrieType* t,std::string const& data) { t->insert(data); return true; } },
        { "search", [](TrieType* t,std::string const& data) { return t->search(data); } },
        { "startsWith", [](TrieType* t,std::string const& data) { return t->startsWith(data); } },
    };

    std::vector<bool> results;
    results.reserve(tests.size());
    for (const auto& [cmd, arg] : tests)
    {
        auto f = cmds.find(cmd);
        if (f != cmds.end())
        {
            bool result = f->second(current, arg);
            std::cout << std::format("{} <- {}('{}')\n", result, cmd, arg);
            results.push_back(result);
        }
    }

    for (TrieType*& t : tries) {
        delete t; t = nullptr;
    }
}

int lengthOfLongestSubstring(const std::string& s) {
    static std::array<int, 256> foundAt;
    foundAt.fill(-1);

    int maxcnt = 0;
    int cnt = 0;
    for (int i = 0; i < s.length(); ++i) {
        const char c = s[i];
        int pos = static_cast<int>(c);
        if (foundAt[pos] == -1) {
            foundAt[pos] = i;
            cnt++;
        }
        else {
            maxcnt = std::max(cnt, maxcnt);
            i = foundAt[pos];
            foundAt.fill(-1);
            cnt = 0;
        }
    }

    maxcnt = std::max(cnt, maxcnt);
    return maxcnt;
}

void runSubStr()
{
    std::vector<std::tuple<std::string, int>> test{
        {"dvdf",3},
        {"cdd",2},
    {"abcabcbb",3},
    {"bbbbb",1},
    {"pwwkew",3},
        {" ", 1},
    };
    for (const auto& [data, num] : test)
    {
        int res = lengthOfLongestSubstring(data);
        if (res != num)
        {
            std::cout << "unexpected\n";
        }
    }
}
#include <numeric>
// https://leetcode.com/problems/median-of-two-sorted-arrays/description/
double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) {
    auto getmiddle = [](std::vector<int>& merged)
    {
        int middle = merged.size() / 2;
        if (merged.size() % 2 == 1)
        {
            return merged[middle];
        }
        else {
            return (merged[middle - 1] + merged[middle]) / 2.0;
        }
        return {};
    };
    struct MinMax {
        int min{};
        int max{};
    };
    if (nums1.size() == 0 && nums2.size() == 0)
    {
        return {};
    }
    else if (nums1.size() == 0)
    {
        return getmiddle(nums2);
    }
    else if (nums2.size() == 0)
    {
        return getmiddle(nums1);
    }

    MinMax mm1 = { nums1[0],nums1[nums1.size() - 1] };
    MinMax mm2 = { nums2[0],nums2[nums2.size() - 1] };
    std::vector<int>& l = mm1.min <= mm2.min ? nums1 : nums2;
    std::vector<int>& r = mm2.min >= mm1.min ? nums2 : nums1;

    mm1= { l[0],l[l.size() - 1] };
    mm2= { r[0],r[r.size() - 1] };

    int sz = l.size() + r.size();
    std::vector<int> merged;
    merged.reserve(sz);
    std::merge(l.begin(), l.end(), r.begin(), r.end(), std::back_inserter(merged));
    return getmiddle(merged);
}

void runMedian()
{
    std::vector < std::tuple<std::vector<int>, std::vector<int>, double>> test{
    {{1,3},{2}, 2.0},
    {{1,2},{3,4}, 2.5},
    };
    for ( auto& [v1, v2, expected]:test) {
        double res = findMedianSortedArrays(v1, v2);
        if (res != expected)
        {
            std::cout << "unexpected\n";
        }
    }
}

int main(int argc, char** argv)
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2023");//Set

	StopWatch clk;
    //runTrie();
    //runSubStr();
    runMedian();
    return 0;
    //Solution s;
    //std::vector<std::vector<int>> input{ std::vector<int>{1,0} };
    //std::vector<int> res=s.findOrder(2, input);

	//aoc2023_01(); clk.printDelta("Day01"); std::cout << "\n\n";
	//aoc2023_02(); clk.printDelta("Day02"); std::cout << "\n\n";
	//aoc2023_03(); clk.printDelta("Day03"); std::cout << "\n\n";
	//aoc2023_04(); clk.printDelta("Day04"); std::cout << "\n\n";
	//aoc2023_05(); clk.printDelta("Day05"); std::cout << "\n\n";
	//aoc2023_06(); clk.printDelta("Day06"); std::cout << "\n\n";
	//aoc2023_07(); clk.printDelta("Day07"); std::cout << "\n\n";
	//aoc2023_08(); clk.printDelta("Day08"); std::cout << "\n\n";
	//aoc2023_09(); clk.printDelta("Day09"); std::cout << "\n\n";
	//aoc2023_10(); clk.printDelta("Day10"); std::cout << "\n\n"; //<-TODO:make nicer
	//aoc2023_11(); clk.printDelta("Day11"); std::cout << "\n\n";
	//aoc2023_12(); clk.printDelta("Day12"); std::cout << "\n\n"; //TODO: skipped in replication
	//aoc2023_13(); clk.printDelta("Day13"); std::cout << "\n\n";

	return 0;
}