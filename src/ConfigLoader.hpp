#ifndef CONFIG_LOADER_HPP
#define CONFIG_LOADER_HPP

#include <string>
#include <unordered_map>
#include "json.hpp"

class ConfigLoader {
public:
    static bool load(const std::string& path);
    static nlohmann::json get(const std::string& key);
    static int getInt(const std::string& key, int def);
    static float getFloat(const std::string& key, float def);
    static bool getBool(const std::string& key, bool def);
    static std::string getString(const std::string& key, const std::string& def);
    static bool isLoaded();
    static void clear();
    static bool validate(const std::unordered_map<std::string, std::string>& requiredKeys);

private:
    static nlohmann::json data;
    static bool loaded;
    static nlohmann::json resolveKey(const std::string& key);
};

#endif // CONFIG_LOADER_HPP
