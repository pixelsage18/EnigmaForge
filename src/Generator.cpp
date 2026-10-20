#include "Generator.h"
#include "CipherPuzzle.h"
#include "CodePuzzle.h"
#include "RiddlePuzzle.h"
#include "SequencePuzzle.h"
#include <queue>
#include <utility>
using namespace std;

// ---------- theme data ----------
const vector<Theme>& Generator::themes() {
    static const vector<Theme> all = {
        { "The Haunted Library",
          { {"The more of me you take, the more you leave behind. What am I?", "footsteps", "Think about taking a walk."},
            {"I speak without a mouth and hear without ears. What am I?", "echo", "Shout into a canyon."},
            {"I have a spine but no bones, and I tell stories. What am I?", "book", "You are standing in a room full of them."},
            {"I follow you all day but vanish in the dark. What am I?", "shadow", "Look at the floor on a sunny day."} },
          {"whisper", "chapter", "candle", "lantern"},
          {"Dusty Bookshelf", "Locked Reading Desk", "Ancient Globe", "Grandfather Clock", "Iron Candelabra", "Brass Lectern"} },

        { "The Abandoned Laboratory",
          { {"What can you catch but never throw?", "cold", "It makes you sneeze."},
            {"I am always hungry and must be fed. The finger I touch soon turns red. What am I?", "fire", "Do not touch it."},
            {"What gets wetter the more it dries?", "towel", "You use it after a shower."},
            {"What has hands but cannot clap?", "clock", "It tells you the time."} },
          {"formula", "reactor", "specimen", "vacuum"},
          {"Cracked Microscope", "Rusty Locker", "Chemical Cabinet", "Humming Console", "Broken Centrifuge", "Steel Safe"} },

        { "The Pirate Ship",
          { {"I have cities but no houses, mountains but no trees, water but no fish. What am I?", "map", "Sailors never leave port without one."},
            {"What has a neck but no head?", "bottle", "A message can float inside it."},
            {"What can travel around the world while staying in a corner?", "stamp", "It goes on letters."},
            {"I am light as a feather, yet the strongest pirate cannot hold me for ten minutes. What am I?", "breath", "You do it all day without noticing."} },
          {"treasure", "anchor", "compass", "cannon"},
          {"Barnacle Chest", "Captain's Desk", "Ship's Wheel", "Powder Barrel", "Salty Cabin Door", "Rope Locker"} }
    };
    return all;
}

string Generator::difficultyName(Difficulty d) {
    switch (d) {
        case Difficulty::Easy:   return "Easy";
        case Difficulty::Medium: return "Medium";
        default:                 return "Hard";
    }
}

// ---------- random helpers ----------
Generator::Generator(unsigned seed) : seed_(seed) {
    if (seed_ == 0) {
        random_device rd;
        seed_ = 100000 + rd() % 900000;      // 6-digit seed, easy to type again
    }
    rng_.seed(seed_);
}

// mt19937's raw output is defined by the C++ standard, so using it with % gives
// the SAME numbers on every compiler/OS. (uniform_int_distribution is not guaranteed to.)
int Generator::randInt(int lo, int hi) {
    return lo + static_cast<int>(static_cast<unsigned>(rng_()) % static_cast<unsigned>(hi - lo + 1));
}

// Fisher-Yates shuffle written by hand for the same portability reason.
vector<int> Generator::shuffledIndices(int n) {
    vector<int> v(n);
    for (int i = 0; i < n; i++) v[i] = i;
    for (int i = n - 1; i > 0; i--) swap(v[i], v[randInt(0, i)]);
    return v;
}

// ---------- puzzle factories ----------
unique_ptr<Puzzle> Generator::makeCode(int id, const string& title, Difficulty d) {
    int digits = (d == Difficulty::Easy) ? 3 : (d == Difficulty::Medium ? 4 : 5);
    string code, sums;
    for (int i = 0; i < digits; i++) {
        int digit = randInt(0, 9);
        code += static_cast<char>('0' + digit);
        string expr;
        if (randInt(0, 1) == 0) {                       // a + b = digit
            int a = randInt(0, digit);
            expr = to_string(a) + " + " + to_string(digit - a);
        } else {                                        // a - b = digit
            int b = randInt(1, 6);
            expr = to_string(digit + b) + " - " + to_string(b);
        }
        sums += "(" + expr + ")  ";
    }
    string clue = "A crumpled note shows each digit of the lock as a small sum:\n  " + sums +
                  "\nSolve each bracket, in order.";
    return make_unique<CodePuzzle>(id, title, "Work out each bracket and write the answers side by side.",
                                   clue, code);
}

unique_ptr<Puzzle> Generator::makeSequence(int id, const string& title, Difficulty d) {
    // 0 = arithmetic, 1 = geometric, 2 = squares, 3 = fibonacci-style
    vector<int> kinds;
    if (d == Difficulty::Easy)        kinds = {0};
    else if (d == Difficulty::Medium) kinds = {0, 1, 2};
    else                              kinds = {1, 2, 3};
    int kind = kinds[randInt(0, static_cast<int>(kinds.size()) - 1)];

    vector<int> shown;
    int answer = 0;
    string hint;
    if (kind == 0) {
        int start = randInt(1, 10), step = randInt(2, 6);
        for (int i = 0; i < 4; i++) shown.push_back(start + step * i);
        answer = start + step * 4;
        hint = "Look at the difference between each pair of neighbours.";
    } else if (kind == 1) {
        int start = randInt(1, 4), ratio = randInt(2, 3), value = start;
        for (int i = 0; i < 4; i++) { shown.push_back(value); value *= ratio; }
        answer = value;
        hint = "Each number is the previous one multiplied by the same amount.";
    } else if (kind == 2) {
        int s = randInt(2, 8);
        for (int i = 0; i < 4; i++) shown.push_back((s + i) * (s + i));
        answer = (s + 4) * (s + 4);
        hint = "Every number is a perfect square.";
    } else {
        shown = {randInt(1, 5), randInt(1, 5)};
        while (shown.size() < 5) shown.push_back(shown[shown.size() - 1] + shown[shown.size() - 2]);
        answer = shown[3] + shown[4];
        hint = "Each number is the sum of the two before it.";
    }
    return make_unique<SequencePuzzle>(id, title, hint, shown, answer);
}

unique_ptr<Puzzle> Generator::makeCipher(int id, const string& title, Difficulty d, const string& word) {
    int shift = (d == Difficulty::Easy) ? randInt(1, 3)
              : (d == Difficulty::Medium) ? randInt(3, 10) : randInt(5, 20);
    return make_unique<CipherPuzzle>(id, title,
        "To decode, move every letter BACKWARDS by the shift amount.", word, shift);
}

unique_ptr<Puzzle> Generator::makeRiddle(int id, const string& title, const RiddleEntry& r) {
    return make_unique<RiddlePuzzle>(id, title, r.hint, r.text + " (one word)", r.answer);
}

// ---------- the main generator ----------
Room Generator::generateRoom(int themeIndex, Difficulty diff) {
    const Theme& theme = themes().at(themeIndex);
    int count = (diff == Difficulty::Easy) ? 4 : (diff == Difficulty::Medium ? 5 : 6);

    Room room(theme.name);

    // Queues of not-yet-used riddles and cipher words, so no puzzle repeats inside a room.
    queue<int> riddleQ, wordQ;
    for (int i : shuffledIndices(static_cast<int>(theme.riddles.size())))     riddleQ.push(i);
    for (int i : shuffledIndices(static_cast<int>(theme.cipherWords.size()))) wordQ.push(i);

    vector<int> objectOrder = shuffledIndices(static_cast<int>(theme.objects.size()));

    // Cycle the 4 puzzle types so every type appears, then shuffle their order.
    vector<int> types;
    for (int i = 0; i < count; i++) types.push_back(i % 4);
    vector<int> order = shuffledIndices(count);

    for (int i = 0; i < count; i++) {
        int id = i + 1;
        string title = theme.objects[objectOrder[i]];
        switch (types[order[i]]) {
            case 0: room.addPuzzle(makeCode(id, title, diff)); break;
            case 1: room.addPuzzle(makeRiddle(id, title, theme.riddles[riddleQ.front()])); riddleQ.pop(); break;
            case 2: room.addPuzzle(makeSequence(id, title, diff)); break;
            default: room.addPuzzle(makeCipher(id, title, diff, theme.cipherWords[wordQ.front()])); wordQ.pop(); break;
        }
    }
        // ----- dependency graph -----
    // Pick a random unlock order. Edges only point from EARLIER to LATER puzzles in this
    // order, so the graph can never contain a cycle. Harder rooms get more dependencies.
    vector<int> unlockOrder = shuffledIndices(count);          // puzzle id = position + 1
    int chance = (diff == Difficulty::Easy) ? 40 : (diff == Difficulty::Medium ? 70 : 90);
    for (int k = 1; k < count; k++) {
        int id = unlockOrder[k] + 1;
        bool mustHave = (k == count - 1);                      // last puzzle always has a prerequisite
        if (!mustHave && randInt(1, 100) > chance) continue;

        room.addDependency(unlockOrder[randInt(0, k - 1)] + 1, id);
        if (diff == Difficulty::Hard && k >= 2 && randInt(1, 100) <= 30)
            room.addDependency(unlockOrder[randInt(0, k - 1)] + 1, id);   // 2nd prerequisite (duplicates ignored)
    }
    return room;
}