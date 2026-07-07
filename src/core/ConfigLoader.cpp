#include "ConfigLoader.hpp"
#include <fstream>
#include <iostream>

nlohmann::json ConfigLoader::data;
bool ConfigLoader::loaded = false;

nlohmann::json ConfigLoader::resolveKey(const std::string& key) {
    if (!loaded || data.is_null()) return nlohmann::json();

    const nlohmann::json* current = &data;
    size_t start = 0;
    while (start < key.size()) {
        size_t dot = key.find('.', start);
        std::string segment = key.substr(start, dot - start);
        if (!current->is_object() || current->find(segment) == current->end()) {
            return nlohmann::json();
        }
        current = &(*current)[segment];
        if (dot == std::string::npos) break;
        start = dot + 1;
    }
    return *current;
}

bool ConfigLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        loaded = false;
        data = nlohmann::json();
        std::cerr << "Config: Could not open '" << path << "'. Using defaults." << std::endl;
        return false;
    }
    try {
        file >> data;
    } catch (const std::exception& e) {
        loaded = false;
        data = nlohmann::json();
        std::cerr << "Config: Parse error in '" << path << "': " << e.what() << ". Using defaults." << std::endl;
        return false;
    }
    loaded = true;

    std::unordered_map<std::string, std::string> required = {
        {"screen_width", "int"}, {"screen_height", "int"},
        {"gravity", "float"}, {"jump_strength", "float"},
        {"pipe_speed", "float"}, {"gap_height", "float"},
        {"pipe_spawn_interval", "float"}, {"ground_height", "int"}
    };
    validate(required);

    return true;
}

nlohmann::json ConfigLoader::get(const std::string& key) {
    return resolveKey(key);
}

int ConfigLoader::getInt(const std::string& key, int def) {
    auto v = resolveKey(key);
    if (v.is_number_integer()) return v.get<int>();
    return def;
}

float ConfigLoader::getFloat(const std::string& key, float def) {
    auto v = resolveKey(key);
    if (v.is_number_float()) return v.get<float>();
    if (v.is_number_integer()) return static_cast<float>(v.get<int>());
    return def;
}

bool ConfigLoader::getBool(const std::string& key, bool def) {
    auto v = resolveKey(key);
    if (v.is_boolean()) return v.get<bool>();
    return def;
}

std::string ConfigLoader::getString(const std::string& key, const std::string& def) {
    auto v = resolveKey(key);
    if (v.is_string()) return v.get<std::string>();
    return def;
}

bool ConfigLoader::isLoaded() { return loaded; }

void ConfigLoader::clear() {
    data = nlohmann::json();
    loaded = false;
}

bool ConfigLoader::validate(const std::unordered_map<std::string, std::string>& requiredKeys) {
    bool allPresent = true;
    for (const auto& [key, type] : requiredKeys) {
        if (resolveKey(key).is_null()) {
            std::cerr << "Config warning: Missing required key '" << key << "' (expected: " << type << "). Using default." << std::endl;
            allPresent = false;
        }
    }
    return allPresent;
}
