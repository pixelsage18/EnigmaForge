#pragma once
#include "Puzzle.h"
#include <vector>
using namespace std;

// Player finds the next number in a pattern, e.g. 2, 4, 8, 16, ?
class SequencePuzzle : public Puzzle {
public:
    SequencePuzzle(int id, string title, string hint, vector<int> shown, int answer);

    string getPrompt() const override;
    bool checkAnswer(const string& answer) override;
    string typeName() const override { return "Sequence"; }

private:
    vector<int> shown_;   // numbers displayed to the player
    int answer_;          // the missing next number
};