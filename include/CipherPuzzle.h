#pragma once
#include "Puzzle.h"
using namespace std;

// A secret word is hidden with a Caesar cipher; the player decodes it.
class CipherPuzzle : public Puzzle {
public:
    CipherPuzzle(int id, string title, string hint, string plaintext, int shift);

    string getPrompt() const override;
    bool checkAnswer(const string& answer) override;
    string typeName() const override { return "Cipher"; }

private:
    static string caesar(const string& text, int shift);  // helper, needs no object

    string plaintext_;
    string ciphertext_;   // computed once in the constructor
    int shift_;
};