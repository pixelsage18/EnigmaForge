#pragma once
#include "DependencyGraph.h"
#include "Puzzle.h"
#include <memory>
#include <string>
#include <vector>
using namespace std;

// Composition: a Room OWNS its puzzles (unique_ptr) and its dependency graph.
class Room {
public:
    explicit Room(string name);

    void addPuzzle(unique_ptr<Puzzle> p);
    bool addDependency(int prereqId, int puzzleId);   // prereq must be solved first
    Puzzle* getPuzzle(int id);                        // nullptr if not found
    const vector<unique_ptr<Puzzle>>& puzzles() const { return puzzles_; }
    const string& getName() const { return name_; }

    bool isUnlocked(int id) const;                    // are all prerequisites solved?
    const vector<int>& prerequisitesOf(int id) const { return graph_.prerequisitesOf(id); }
    bool isEscaped() const;                           // true when all puzzles solved

    // Solvability validation (delegates to the graph)
    bool isSolvable() const { return graph_.isSolvable(); }
    vector<int> solveOrder() const { return graph_.solveOrder(); }

private:
    string name_;
    vector<unique_ptr<Puzzle>> puzzles_;
    DependencyGraph graph_;
};