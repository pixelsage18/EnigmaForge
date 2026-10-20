#include "Puzzle.h"
#include <utility>
using namespace std;

Puzzle::Puzzle(int id, string title, string hint)
    : id_(id), title_(move(title)), hint_(move(hint)) {}