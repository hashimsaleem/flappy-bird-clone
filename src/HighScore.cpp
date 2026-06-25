#include "HighScore.hpp"
#include <fstream>
#include <iostream>

int HighScore::cached = 0;
bool HighScore::loaded = false;
std::string HighScore::filePath = HighScore::DEFAULT_PATH;

int HighScore::loadFromFile() {
    std::ifstream file(filePath, std::ios::binary);
    if (file.is_open() && file.good()) {
        int score = 0;
        if (file.read(reinterpret_cast<char*>(&score), sizeof(score))) {
            return score;
        }
    }
    return 0;
}

int HighScore::load() {
    if (!loaded) {
        cached = loadFromFile();
        loaded = true;
    }
    return cached;
}

void HighScore::save(int score) {
    int current = load();
    if (score > current) {
        cached = score;
        std::ofstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(&score), sizeof(score));
        } else {
            std::cerr << "Warning: Could not save high score to '" << filePath << "'." << std::endl;
        }
    }
}

int HighScore::get() {
    return load();
}

void HighScore::reset() {
    cached = 0;
    loaded = false;
}

void HighScore::setPath(const std::string& path) {
    filePath = path;
    loaded = false;
}
