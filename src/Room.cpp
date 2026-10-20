#include "Room.h"
#include <utility>
using namespace std;

Room::Room(string name) : name_(move(name)) {}

void Room::addPuzzle(unique_ptr<Puzzle> p) {
    graph_.addNode(p->getId());          // register the puzzle as a graph node
    puzzles_.push_back(move(p));
}

bool Room::addDependency(int prereqId, int puzzleId) {
    return graph_.addEdge(prereqId, puzzleId);
}

Puzzle* Room::getPuzzle(int id) {
    for (auto& p : puzzles_)
        if (p->getId() == id) return p.get();
    return nullptr;
}

bool Room::isUnlocked(int id) const {
    for (int pre : graph_.prerequisitesOf(id))
        for (const auto& p : puzzles_)
            if (p->getId() == pre && !p->isSolved()) return false;
    return true;
}

bool Room::isEscaped() const {
    for (const auto& p : puzzles_)
        if (!p->isSolved()) return false;
    return !puzzles_.empty();
}