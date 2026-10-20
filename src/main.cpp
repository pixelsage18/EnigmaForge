#include "DependencyGraph.h"
#include "Generator.h"
#include "Room.h"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

static void showRoom(const Room& room) {
    cout << "\n=== " << room.getName() << " ===\n";
    for (const auto& p : room.puzzles()) {
        cout << "  [" << p->getId() << "] " << p->getTitle() << " (" << p->typeName() << ") - ";
        if (p->isSolved()) {
            cout << "SOLVED";
        } else if (room.isUnlocked(p->getId())) {
            cout << "open";
        } else {
            cout << "LOCKED (solve first:";
            for (int pre : room.prerequisitesOf(p->getId())) cout << " " << pre;
            cout << ")";
        }
        cout << "\n";
    }
}

// Asks until the player types a number between lo and hi.
static int askChoice(const string& prompt, int lo, int hi) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) { cout << "\n"; exit(0); }
        try {
            int v = stoi(line);
            if (v >= lo && v <= hi) return v;
        } catch (...) {}
        cout << "Please enter a number from " << lo << " to " << hi << ".\n";
    }
}

static bool playRoom(Room& room) {
    while (!room.isEscaped()) {
        showRoom(room);
        cout << "\nChoose puzzle (or 'q' to quit) > ";
        string input;
        if (!getline(cin, input) || input == "q") { cout << "Bye!\n"; return false; }

        int id = 0;
        try { id = stoi(input); } catch (...) { cout << "Enter a number.\n"; continue; }

        Puzzle* p = room.getPuzzle(id);            // base-class pointer
        if (!p) { cout << "No such puzzle.\n"; continue; }
        if (p->isSolved()) { cout << "Already solved.\n"; continue; }
        if (!room.isUnlocked(id)) { cout << "That one is locked. Solve its prerequisites first.\n"; continue; }

        cout << "\n" << p->getPrompt() << "\n(type 'hint' for a hint) > ";
        string ans;
        getline(cin, ans);
        if (ans == "hint") { cout << "Hint: " << p->getHint() << "\n"; continue; }

        // Runtime polymorphism: the right checkAnswer() runs based on the real object type
        cout << (p->checkAnswer(ans) ? "Click! Correct!\n" : "Wrong answer.\n");
    }
    return true;
}

// Demonstrates that the validator really catches impossible rooms.
static void runSelfTest() {
    cout << "--- Self-test: solvability validation ---\n";
    DependencyGraph good;
    for (int i = 1; i <= 4; i++) good.addNode(i);
    good.addEdge(1, 2); good.addEdge(1, 3); good.addEdge(2, 4); good.addEdge(3, 4);
    cout << "Graph A (1->2, 1->3, 2->4, 3->4): " << (good.isSolvable() ? "SOLVABLE" : "BROKEN") << ", order:";
    for (int id : good.solveOrder()) cout << " " << id;

    DependencyGraph bad;
    for (int i = 1; i <= 4; i++) bad.addNode(i);
    bad.addEdge(1, 2); bad.addEdge(2, 3); bad.addEdge(3, 2);   // 2 and 3 wait for each other!
    cout << "\nGraph B (1->2, 2->3, 3->2):       " << (bad.isSolvable() ? "SOLVABLE" : "BROKEN") << ", stuck puzzles:";
    for (int id : bad.unsolvable()) cout << " " << id;
    cout << "\n";
}

int main(int argc, char** argv) {
    if (argc > 1 && string(argv[1]) == "--selftest") { runSelfTest(); return 0; }

    cout << "=== ENIGMAFORGE: Procedural Escape Room ===\n\nThemes:\n";
    const auto& themes = Generator::themes();
    for (size_t i = 0; i < themes.size(); i++)
        cout << "  " << i + 1 << ". " << themes[i].name << "\n";
    int theme = askChoice("Pick a theme > ", 1, static_cast<int>(themes.size())) - 1;

    cout << "\nDifficulty:  1. Easy   2. Medium   3. Hard\n";
    Difficulty diff = static_cast<Difficulty>(askChoice("Pick difficulty > ", 1, 3) - 1);

    cout << "\nSeed (press Enter for a random one) > ";
    string seedText;
    getline(cin, seedText);
    unsigned seed = 0;
    try { seed = static_cast<unsigned>(stoul(seedText)); } catch (...) { seed = 0; }

    Generator gen(seed);
    Room room = gen.generateRoom(theme, diff);
    cout << "\nGenerated " << Generator::difficultyName(diff) << " room with seed "
         << gen.getSeed() << "  (enter this seed again to replay the same room)\n";

    cout << "Engine check: ";
    if (room.isSolvable()) {
        cout << "room is SOLVABLE. One valid order:";
        for (int id : room.solveOrder()) cout << " " << id;
        cout << "\n";
    } else {
        cout << "room is NOT solvable - generator bug!\n";
        return 1;
    }

    if (playRoom(room))
        cout << "\n*** You escaped " << room.getName() << "! ***\n";
}
