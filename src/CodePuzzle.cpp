#include "CodePuzzle.h"
#include <utility>
using namespace std;

CodePuzzle::CodePuzzle(int id, string title, string hint, string clue, string code)
    : Puzzle(id, move(title), move(hint)),
      clue_(move(clue)), code_(move(code)) {}

string CodePuzzle::getPrompt() const {
    return clue_ + "\nEnter the " + to_string(code_.size()) + "-digit code:";
}

bool CodePuzzle::checkAnswer(const string& answer) {
    if (answer == code_) { markSolved(); return true; }
    return false;
}