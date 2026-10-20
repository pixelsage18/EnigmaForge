#include "httplib.h"
#include "Generator.h"
#include "Room.h"
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
using namespace std;

// ---- the one game currently being played (single-player demo) ----
struct Game {
    unique_ptr<Room> room;
    unsigned seed = 0;
    Difficulty diff = Difficulty::Easy;
    int hintsUsed = 0;
    int wrongAttempts = 0;
};
static Game game;
static mutex gameMutex;   // the server handles requests on several threads

// Makes a string safe to place inside JSON quotes.
static string jsonEscape(const string& s) {
    string out;
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            default:   out += c;
        }
    }
    return out;
}

static string intList(const vector<int>& v) {
    string s = "[";
    for (size_t i = 0; i < v.size(); i++) s += (i ? "," : "") + to_string(v[i]);
    return s + "]";
}

static const char* statusOf(const Room& room, const Puzzle& p) {
    if (p.isSolved()) return "solved";
    return room.isUnlocked(p.getId()) ? "open" : "locked";
}

// Turns the current game into JSON. Note: answers are NEVER included.
static string gameToJson(const Game& g) {
    const Room& room = *g.room;
    string j = "{\"name\":\"" + jsonEscape(room.getName()) + "\"";
    j += ",\"seed\":" + to_string(g.seed);
    j += ",\"difficulty\":\"" + Generator::difficultyName(g.diff) + "\"";
    j += string(",\"solvable\":") + (room.isSolvable() ? "true" : "false");
    j += ",\"solveOrder\":" + intList(room.solveOrder());
    j += ",\"escaped\":" + string(room.isEscaped() ? "true" : "false");
    j += ",\"hintsUsed\":" + to_string(g.hintsUsed);
    j += ",\"wrongAttempts\":" + to_string(g.wrongAttempts);
    j += ",\"puzzles\":[";
    bool first = true;
    for (const auto& p : room.puzzles()) {
        if (!first) j += ",";
        first = false;
        j += "{\"id\":" + to_string(p->getId());
        j += ",\"title\":\"" + jsonEscape(p->getTitle()) + "\"";
        j += ",\"type\":\"" + jsonEscape(p->typeName()) + "\"";
        j += string(",\"status\":\"") + statusOf(room, *p) + "\"";
        j += ",\"needs\":" + intList(room.prerequisitesOf(p->getId())) + "}";
    }
    return j + "]}";
}

static void sendError(httplib::Response& res, int code, const string& message) {
    res.status = code;
    res.set_content("{\"error\":\"" + jsonEscape(message) + "\"}", "application/json");
}

// Reads an integer query parameter, or returns the default if it is missing.
static bool readInt(const httplib::Request& req, const string& name, long def, long& out) {
    if (!req.has_param(name)) { out = def; return true; }
    const string text = req.get_param_value(name);
    if (text.empty() || text.size() > 9 || text.find_first_not_of("0123456789") != string::npos) return false;
    out = stol(text);
    return true;
}

// Finds the puzzle named in the URL. Caller must already hold gameMutex.
// On any problem it fills in the error response and returns nullptr.
static Puzzle* findOpenPuzzle(const httplib::Request& req, httplib::Response& res) {
    if (!game.room) { sendError(res, 404, "no game yet - call /api/new first"); return nullptr; }
    int id = 0;
    try { id = stoi(req.matches[1].str()); } catch (...) { sendError(res, 400, "bad puzzle id"); return nullptr; }
    Puzzle* p = game.room->getPuzzle(id);
    if (!p) { sendError(res, 404, "no such puzzle"); return nullptr; }
    if (!game.room->isUnlocked(id)) { sendError(res, 403, "that puzzle is locked - solve its prerequisites first"); return nullptr; }
    return p;
}

int main() {
    httplib::Server svr;
    svr.set_mount_point("/", "./web");   // serves web/index.html at http://localhost:8080/

    // GET or POST /api/new?theme=1..3&difficulty=1..3&seed=N   (seed optional)
    auto newGame = [](const httplib::Request& req, httplib::Response& res) {
        long theme = 0, diffNum = 0, seed = 0;
        const long themeCount = static_cast<long>(Generator::themes().size());
        if (!readInt(req, "theme", 1, theme) || theme < 1 || theme > themeCount)
            return sendError(res, 400, "theme must be a number from 1 to " + to_string(themeCount));
        if (!readInt(req, "difficulty", 1, diffNum) || diffNum < 1 || diffNum > 3)
            return sendError(res, 400, "difficulty must be 1, 2 or 3");
        if (!readInt(req, "seed", 0, seed))
            return sendError(res, 400, "seed must be a whole number");

        Generator gen(static_cast<unsigned>(seed));
        Difficulty diff = static_cast<Difficulty>(diffNum - 1);
        auto room = make_unique<Room>(gen.generateRoom(static_cast<int>(theme) - 1, diff));

        lock_guard<mutex> lock(gameMutex);
        game.room = move(room);
        game.seed = gen.getSeed();
        game.diff = diff;
        game.hintsUsed = 0;
        game.wrongAttempts = 0;
        res.set_content(gameToJson(game), "application/json");
    };
    svr.Get("/api/new", newGame);
    svr.Post("/api/new", newGame);

    // GET /api/room : the current game state
    svr.Get("/api/room", [](const httplib::Request&, httplib::Response& res) {
        lock_guard<mutex> lock(gameMutex);
        if (!game.room) return sendError(res, 404, "no game yet - call /api/new first");
        res.set_content(gameToJson(game), "application/json");
    });

    // GET /api/puzzle/N : the puzzle's prompt (only if it is unlocked)
    svr.Get(R"(/api/puzzle/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(gameMutex);
        Puzzle* p = findOpenPuzzle(req, res);
        if (!p) return;
        string j = "{\"id\":" + to_string(p->getId());
        j += ",\"title\":\"" + jsonEscape(p->getTitle()) + "\"";
        j += ",\"type\":\"" + jsonEscape(p->typeName()) + "\"";
        j += string(",\"status\":\"") + statusOf(*game.room, *p) + "\"";
        j += ",\"prompt\":\"" + jsonEscape(p->getPrompt()) + "\"}";
        res.set_content(j, "application/json");
    });

    // GET /api/puzzle/N/hint : the hint (counted in the statistics)
    svr.Get(R"(/api/puzzle/(\d+)/hint)", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(gameMutex);
        Puzzle* p = findOpenPuzzle(req, res);
        if (!p) return;
        game.hintsUsed++;
        res.set_content("{\"hint\":\"" + jsonEscape(p->getHint()) + "\",\"hintsUsed\":" +
                        to_string(game.hintsUsed) + "}", "application/json");
    });

    // POST /api/puzzle/N/answer   with form field or query parameter  answer=...
    svr.Post(R"(/api/puzzle/(\d+)/answer)", [](const httplib::Request& req, httplib::Response& res) {
        lock_guard<mutex> lock(gameMutex);
        Puzzle* p = findOpenPuzzle(req, res);
        if (!p) return;
        if (p->isSolved()) return sendError(res, 409, "already solved");
        if (!req.has_param("answer")) return sendError(res, 400, "missing 'answer'");
        const string answer = req.get_param_value("answer");
        if (answer.size() > 200) return sendError(res, 400, "answer too long");

        bool correct = p->checkAnswer(answer);    // runtime polymorphism, same as the console game
        if (!correct) game.wrongAttempts++;
        res.set_content(string("{\"correct\":") + (correct ? "true" : "false") +
                        ",\"escaped\":" + (game.room->isEscaped() ? "true" : "false") +
                        ",\"wrongAttempts\":" + to_string(game.wrongAttempts) + "}", "application/json");
    });

    cout << "EnigmaForge web server running at http://localhost:8080  (Ctrl+C to stop)\n";
    if (!svr.listen("127.0.0.1", 8080)) {
        cerr << "Could not start the server. Is port 8080 already in use?\n";
        return 1;
    }
}