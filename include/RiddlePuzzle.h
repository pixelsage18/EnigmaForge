#pragma once
#include "Puzzle.h"
using namespace std;

// Player answers a riddle with a word or phrase (case-insensitive).
class RiddlePuzzle : public Puzzle {
public:
    RiddlePuzzle(int id, string title, string hint, string riddle, string answer);

    string getPrompt() const override;
    bool checkAnswer(const string& answer) override;
    string typeName() const override { return "Riddle"; }

private:
    string riddle_;
    string answer_;
};