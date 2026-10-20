#!/bin/bash
# Builds the web version (all engine files except the console main.cpp).
g++ -std=c++17 -Wall -Wextra -Iinclude -Ithird_party src/web/main_web.cpp \
    $(ls src/*.cpp | grep -v 'src/main.cpp') -o enigmaforge_web -pthread