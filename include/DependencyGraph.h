#pragma once
#include <map>
#include <vector>
using namespace std;

// Directed graph of puzzle dependencies.
// Edge  A -> B  means "puzzle A must be solved BEFORE puzzle B can be attempted".
class DependencyGraph {
public:
    void addNode(int id);
    bool addEdge(int prereq, int puzzle);                 // false if invalid or duplicate

    const vector<int>& prerequisitesOf(int id) const;     // empty if none
    size_t nodeCount() const { return prereqs_.size(); }

    // BFS (Kahn's algorithm) simulating a perfect player: repeatedly "solve"
    // every puzzle whose prerequisites are all done. Returns the order solved.
    vector<int> solveOrder() const;
    bool isSolvable() const;                              // did the simulation solve EVERYTHING?
    vector<int> unsolvable() const;                       // puzzles the simulation could never reach

private:
    map<int, vector<int>> prereqs_;   // puzzle id -> puzzles needed before it
    map<int, vector<int>> unlocks_;   // puzzle id -> puzzles it helps unlock
};