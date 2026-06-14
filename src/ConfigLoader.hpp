#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include <string>
#include <unordered_map>

/**
 * Minimal JSON config loader for game tuning.
 * Reads a flat or slightly nested JSON file and stores values in a map.
 * Supports: int, float, double, bool, string.
 * Falls back to defaults if the file is missing or unparseable.
 */
class ConfigLoader {
public:
    struct Value {
        enum Type { NONE, INT, FLOAT, BOOL, STRING } type = NONE;
        int intValue = 0;
        float floatValue = 0.0f;
        bool boolValue = false;
        std::string stringValue;

        bool isInt() const { return type == INT; }
        bool isFloat() const { return type == FLOAT; }
        bool isBool() const { return type == BOOL; }
        bool isString() const { return type == STRING; }
        bool isNone() const { return type == NONE; }
    };

    /// Load config from a JSON file. Returns true on success.
    static bool load(const std::string& path);

    /// Get a value by key. Returns a Value with type NONE if not found.
    static Value get(const std::string& key);

    /// Convenience accessors with defaults.
    static int getInt(const std::string& key, int def);
    static float getFloat(const std::string& key, float def);
    static bool getBool(const std::string& key, bool def);
    static std::string getString(const std::string& key, const std::string& def);

    /// Check if config was successfully loaded.
    static bool isLoaded();

    static void clear();

    /// Validate that required keys exist. Logs warnings for missing keys.
    static bool validate(const std::unordered_map<std::string, std::string>& requiredKeys);

private:
    static std::unordered_map<std::string, Value> data;
    static bool loaded;

    static void parse(const std::string& json);
    static Value parseValue(const std::string& s);
    static std::string trim(const std::string& s);
    static std::string unescape(const std::string& s);
};

#endif // CONFIG_LOADER_HPP
