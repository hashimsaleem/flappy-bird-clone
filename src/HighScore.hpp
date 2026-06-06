#ifndef HIGH_SCORE_HPP
#define HIGH_SCORE_HPP

class HighScore {
public:
    static int load();
    static void save(int score);
    static int get();
    static void reset();

private:
    static constexpr const char* PATH = "highscore.dat";
    static int cached;
    static bool loaded;

    static int loadFromFile();
};

#endif // HIGH_SCORE_HPP
