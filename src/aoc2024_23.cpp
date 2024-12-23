#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>
#include <vector>
#include <map>
#include <set>
#include <print>

namespace D23 {
	using ID = std::array<char, 2>;
	struct Node {
		ID id{ '.','.'};
		std::vector<ID> connections{};
		bool hasFriend(ID idOther) const{
			return std::find(
				connections.begin(), 
				connections.end(), 
				idOther) != connections.end();
		}
	};

	constexpr const size_t id_range = 'z' - 'a' + 1;
	struct NodePool final {
		using Row = std::vector<Node>;
		std::vector<Row> nodesByName{};
		NodePool(){
			nodesByName.resize(id_range);
			for (Row& row : nodesByName)
			{
				row.resize(id_range);
			}
		}
		Node& get(const ID id){
			int row = id[0]-'a';
			int col = id[1]-'a';
			return nodesByName[row][col];
		}

		const Node& get(const ID id) const {
			int row = id[0] - 'a';
			int col = id[1] - 'a';
			return nodesByName[row][col];
		}

		//test: faster precalculated keys? without -a
		///*Node& get(const keyID id){
		//	int row = id[0];
		//	int col = id[1];
		//	return nodesByName[row][col];
		//*/}
		void print() const {
			std::stringstream buf;
			for (char c1 = 'a'; c1 <= 'z'; ++c1) {
				for (char c2 = 'a'; c2 <= 'z'; ++c2) {
					const Node& n = get({ c1,c2 });
					std::print(buf, "[{:c}{:c} {:<3}]", n.id[0], n.id[1], n.connections.size());
				}
				std::print(buf, "\n");
			}
			std::println(std::cout, "{}", buf.str());
		}
		
	};

	int appendDirectFriends(const Node& t, const NodePool& data, std::set<std::array<ID, 3>>& friends)
	{
		int buddyCount{};
		for (int i = 0; i < t.connections.size(); ++i) {
			const ID& idCur = t.connections[i];
			const Node& buddy1 = data.get(idCur);
			for (int j = i + 1; j < t.connections.size(); ++j) {
				const ID& idOther = t.connections[j];
				if (buddy1.hasFriend(idOther)) {
					std::array<ID, 3> triple{ t.id,idCur,idOther };
					std::sort(triple.begin(), triple.end());
					if (!friends.contains(triple))
					{
						friends.emplace(triple);
						buddyCount++;
					}
				}
			}
		}
		return buddyCount;
	}
}

void aoc2024_23()
{
	using namespace D23;
	fs::path input(DataDir() / "2024_23.txt");
	TxtFile txt{ input };

	NodePool pool{};

	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		if (line.length() > 5) { continue; }
		ID id1 = { line[0],line[1] };
		ID id2 = { line[3],line[4] };
		
		Node& n1 = pool.get(id1);
		Node& n2 = pool.get(id2);
		n1.id = id1;
		n2.id = id2;
		n1.connections.push_back(id2);
		n2.connections.push_back(id1);
	}
#ifndef NDEBUG
	pool.print();
#endif
	std::vector < std::tuple<ID, int> > buddyCounts{};
	for (char row = 'a'; row <= 'z';++row) {
		const NodePool::Row& T = pool.nodesByName[row-'a'];
		std::set<std::array<ID, 3>> directFriends{};
		for (const Node& t : T) {
			int buddyCount = appendDirectFriends(t, pool, directFriends);
			buddyCounts.emplace_back(t.id, buddyCount);
		}
		if (row == 't') {
			std::println(std::cout, "triangles {}", directFriends.size());
#ifndef NDEBUG
			for (const auto& triple : directFriends)
			{
				std::println(std::cout, "{}", triple);
			}
#endif
		}
	}

	std::sort(buddyCounts.begin(), buddyCounts.end(), 
		[](std::tuple<ID,int>& l, std::tuple<ID, int>& r) {
			return std::get<1>(l) > std::get<1>(r);
		});

	std::map<ID, int> hist{};
	int last = -1;
	for (const auto& [name, cnt] : buddyCounts) {
		if (last == -1) { last = cnt; }
		if (cnt != last) { break; }
		last=cnt;
#ifndef NDEBUG
		std::println(std::cout, "{} {}", name, cnt);
#endif
		Node& n=pool.get(name);
		n.connections.push_back(n.id);
		std::sort(n.connections.begin(), n.connections.end());
		for (const ID c : n.connections) {
			hist[c]++;
#ifndef NDEBUG
			std::print(std::cout, "{} ", c);
#endif
		}
#ifndef NDEBUG
		std::println(std::cout);
#endif
	}

	int maxcnt{};
	for (const auto& [id, cnt] : hist) {
#ifndef NDEBUG
		std::println(std::cout, "{} {}", id,cnt);
#endif
		maxcnt = std::max(cnt, maxcnt);
	}

	for (bool first = true; const auto & [id, cnt] : hist) {
		if (cnt == maxcnt) {
			std::print("{}{}{}", (first?"":","), id[0], id[1]);
			first = false;
		}
	}
	std::println(std::cout);

}

