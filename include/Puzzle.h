#pragma once
#include <string>
using namespace std;

// Abstract base class: defines WHAT every puzzle can do, not HOW.
class Puzzle {
public:
    Puzzle(int id, string title, string hint);
    virtual ~Puzzle() = default;           // virtual destructor: safe delete via base pointer

    // Pure virtual functions: every derived puzzle MUST implement these.
    virtual string getPrompt() const = 0;
    virtual bool checkAnswer(const string& answer) = 0;
    virtual string typeName() const = 0;

    // Shared behaviour (inherited as-is)
    int getId() const { return id_; }
    const string& getTitle() const { return title_; }
    const string& getHint() const { return hint_; }
    bool isSolved() const { return solved_; }

protected:
    void markSolved() { solved_ = true; }  // only derived classes may call this

private:
    int id_;
    string title_;
    string hint_;
    bool solved_ = false;                  // encapsulated
};