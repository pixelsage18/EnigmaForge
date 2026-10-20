#include "CipherPuzzle.h"
#include "Utils.h"
#include <utility>
using namespace std;

CipherPuzzle::CipherPuzzle(int id, string title, string hint, string plaintext, int shift)
    : Puzzle(id, move(title), move(hint)),
      plaintext_(toLowerTrim(plaintext)), shift_(shift) {
    ciphertext_ = caesar(plaintext_, shift_);
}

// Shifts each letter forward by 'shift' (wraps around z -> a). Non-letters unchanged.
string CipherPuzzle::caesar(const string& text, int shift) {
    string out = text;
    for (char& c : out) {
        if (c >= 'a' && c <= 'z')
            c = static_cast<char>('a' + ((c - 'a' + shift) % 26 + 26) % 26);
    }
    return out;
}

string CipherPuzzle::getPrompt() const {
    return "A coded message is scratched on the door:\n  " + ciphertext_ +
           "\nEvery letter was shifted forward by " + to_string(shift_) +
           " in the alphabet. Decode the message:";
}

bool CipherPuzzle::checkAnswer(const string& answer) {
    if (toLowerTrim(answer) == plaintext_) { markSolved(); return true; }
    return false;
}