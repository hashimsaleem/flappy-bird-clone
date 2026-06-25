#ifndef HIGH_SCORE_HPP
#define HIGH_SCORE_HPP

#include <string>

class HighScore {
public:
    static int load();
    static void save(int score);
    static int get();
    static void reset();
    static void setPath(const std::string& path);

private:
    static constexpr const char* DEFAULT_PATH = "highscore.dat";
    static std::string filePath;
    static int cached;
    static bool loaded;

    static int loadFromFile();
};

#endif // HIGH_SCORE_HPP
