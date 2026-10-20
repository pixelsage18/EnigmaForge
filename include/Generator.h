#pragma once
#include "Room.h"
#include <random>
#include <string>
#include <vector>
using namespace std;

enum class Difficulty { Easy, Medium, Hard };

struct RiddleEntry {
    string text;
    string answer;   // one lowercase word
    string hint;
};

// A theme is just DATA: names, riddle bank, cipher words. Adding a theme = adding data.
struct Theme {
    string name;
    vector<RiddleEntry> riddles;
    vector<string> cipherWords;
    vector<string> objects;       // used as puzzle titles ("Dusty Bookshelf")
};

// Builds a random Room. Same seed + same choices => exactly the same Room.
class Generator {
public:
    explicit Generator(unsigned seed);               // seed 0 => pick a random seed
    Room generateRoom(int themeIndex, Difficulty diff);

    unsigned getSeed() const { return seed_; }
    static const vector<Theme>& themes();
    static string difficultyName(Difficulty d);

private:
    int randInt(int lo, int hi);                     // inclusive range
    vector<int> shuffledIndices(int n);              // 0..n-1 in random order

    unique_ptr<Puzzle> makeCode(int id, const string& title, Difficulty d);
    unique_ptr<Puzzle> makeSequence(int id, const string& title, Difficulty d);
    unique_ptr<Puzzle> makeCipher(int id, const string& title, Difficulty d, const string& word);
    unique_ptr<Puzzle> makeRiddle(int id, const string& title, const RiddleEntry& r);

    unsigned seed_;
    mt19937 rng_;
};