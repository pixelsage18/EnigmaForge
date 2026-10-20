#pragma once
#include "Puzzle.h"
using namespace std;

// Player must enter a numeric code (e.g. "4721") found from a clue.
class CodePuzzle : public Puzzle {
public:
    CodePuzzle(int id, string title, string hint, string clue, string code);

    string getPrompt() const override;
    bool checkAnswer(const string& answer) override;
    string typeName() const override { return "Code Lock"; }

private:
    string clue_;
    string code_;
};