#include "DependencyGraph.h"
#include <algorithm>
#include <queue>
#include <set>
using namespace std;

void DependencyGraph::addNode(int id) {
    prereqs_[id];   // operator[] creates an empty entry if missing
    unlocks_[id];
}

bool DependencyGraph::addEdge(int prereq, int puzzle) {
    if (prereq == puzzle) return false;                                  // self-loop
    if (!prereqs_.count(prereq) || !prereqs_.count(puzzle)) return false; // unknown node
    vector<int>& list = prereqs_[puzzle];
    if (find(list.begin(), list.end(), prereq) != list.end()) return false; // duplicate
    list.push_back(prereq);
    unlocks_[prereq].push_back(puzzle);
    return true;
}

const vector<int>& DependencyGraph::prerequisitesOf(int id) const {
    static const vector<int> none;
    auto it = prereqs_.find(id);
    return it == prereqs_.end() ? none : it->second;
}

vector<int> DependencyGraph::solveOrder() const {
    map<int, int> remaining;     // how many prerequisites are still unsolved
    queue<int> ready;            // puzzles the player could solve right now
    vector<int> order;

    for (const auto& entry : prereqs_) {
        remaining[entry.first] = static_cast<int>(entry.second.size());
        if (entry.second.empty()) ready.push(entry.first);   // starting puzzles
    }
    while (!ready.empty()) {
        int current = ready.front();
        ready.pop();
        order.push_back(current);                            // "solve" it
        for (int next : unlocks_.at(current))
            if (--remaining[next] == 0) ready.push(next);    // all prerequisites done -> now open
    }
    return order;   // anything missing here is stuck behind a cycle
}

bool DependencyGraph::isSolvable() const {
    return solveOrder().size() == prereqs_.size();
}

vector<int> DependencyGraph::unsolvable() const {
    vector<int> order = solveOrder();
    set<int> reached(order.begin(), order.end());
    vector<int> stuck;
    for (const auto& entry : prereqs_)
        if (!reached.count(entry.first)) stuck.push_back(entry.first);
    return stuck;
}
