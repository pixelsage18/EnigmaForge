#include "SequencePuzzle.h"
#include <utility>
using namespace std;

SequencePuzzle::SequencePuzzle(int id, string title, string hint, vector<int> shown, int answer)
    : Puzzle(id, move(title), move(hint)),
      shown_(move(shown)), answer_(answer) {}

string SequencePuzzle::getPrompt() const {
    string s = "Strange numbers are carved into the wall:\n  ";
    for (int n : shown_) s += to_string(n) + ", ";
    s += "?\nWhat number comes next?";
    return s;
}

bool SequencePuzzle::checkAnswer(const string& answer) {
    try {
        size_t used = 0;
        int value = stoi(answer, &used);
        if (used == answer.size() && value == answer_) { markSolved(); return true; }
    } catch (...) {
        // not a number -> simply wrong
    }
    return false;
}