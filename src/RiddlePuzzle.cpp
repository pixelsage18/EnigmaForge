#include "RiddlePuzzle.h"
#include "Utils.h"
#include <utility>
using namespace std;

RiddlePuzzle::RiddlePuzzle(int id, string title, string hint, string riddle, string answer)
    : Puzzle(id, move(title), move(hint)),
      riddle_(move(riddle)), answer_(toLowerTrim(answer)) {}

string RiddlePuzzle::getPrompt() const {
    return riddle_ + "\nYour answer:";
}

bool RiddlePuzzle::checkAnswer(const string& answer) {
    if (toLowerTrim(answer) == answer_) { markSolved(); return true; }
    return false;
}