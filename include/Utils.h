#pragma once
#include <algorithm>
#include <cctype>
#include <string>
using namespace std;

// Small helpers shared by puzzles. 'inline' lets us define them in a header.
inline string toLowerTrim(const string& s) {
    size_t start = 0, end = s.size();
    while (start < end && isspace(static_cast<unsigned char>(s[start]))) start++;
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) end--;
    string out = s.substr(start, end - start);
    transform(out.begin(), out.end(), out.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return out;
}
