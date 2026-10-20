# EnigmaForge

A procedural escape room engine written in C++17. Instead of one fixed game, it
generates a new room every time: random puzzles, random clues, themes, difficulty
levels, and puzzles that unlock in a random order. Playable in the console or in a
browser.

## Features

- **Four puzzle types**: code lock, riddle, number sequence and Caesar cipher
- **Procedural generation**: 3 themes and 3 difficulty levels
- **Reproducible seeds**: the same seed always rebuilds the same room
- **Dependency graph**: some puzzles stay locked until others are solved
- **Solvability check**: a BFS (Kahn's algorithm) proves every room can be finished
- **Web version**: a small C++ HTTP server plus an HTML/JS page

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Abstraction | `Puzzle` is an abstract base class with pure virtual functions |
| Inheritance | `CodePuzzle`, `RiddlePuzzle`, `SequencePuzzle`, `CipherPuzzle` derive from `Puzzle` |
| Runtime polymorphism | The game calls `checkAnswer()` through a `Puzzle*` |
| Encapsulation | Private data, with `markSolved()` protected so only puzzles can change it |
| Composition | `Room` owns its puzzles (`unique_ptr`) and a `DependencyGraph` |
| STL | `vector`, `map`, `queue`, `set`, `unique_ptr` |
| Graph traversal | `DependencyGraph::solveOrder()` uses BFS with a queue |

## Project structure

```
include/          header files (.h)
src/              implementation files (.cpp) and the console main.cpp
src/web/          main_web.cpp, the web server
web/index.html    the browser page
third_party/      httplib.h (cpp-httplib, MIT license)
```

## Build and run

Requires a C++17 compiler (g++ or clang). Tested on macOS.

**Console game**
```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/*.cpp -o enigmaforge
./enigmaforge
```
Run `./enigmaforge --selftest` to see the solvability checker catch an impossible room.

**Web version**
```bash
./build_web.sh
./enigmaforge_web
```
Then open http://localhost:8080 in a browser.

## How to play

Choose a theme, a difficulty and an optional seed. Pick a puzzle, read its clue and
type your answer, or ask for a hint. Locked puzzles open once their prerequisites are
solved. Solve every puzzle to escape. Riddle answers are a single word.

## Credits

HTTP server library: [cpp-httplib](https://github.com/yhirose/cpp-httplib) by Yuji Hirose (MIT).
