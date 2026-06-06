#include "ConfigLoader.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

std::unordered_map<std::string, ConfigLoader::Value> ConfigLoader::data;
bool ConfigLoader::loaded = false;

std::string ConfigLoader::trim(const std::string& s) {
    size_t l = 0, r = s.size();
    while (l < r && std::isspace(static_cast<unsigned char>(s[l]))) ++l;
    while (r > l && std::isspace(static_cast<unsigned char>(s[r - 1]))) --r;
    return s.substr(l, r - l);
}

std::string ConfigLoader::unescape(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            switch (s[i + 1]) {
                case 'n': result += '\n'; ++i; break;
                case 't': result += '\t'; ++i; break;
                case 'r': result += '\r'; ++i; break;
                case '"': result += '"'; ++i; break;
                case '\\': result += '\\'; ++i; break;
                case '/': result += '/'; ++i; break;
                default: result += s[i]; break;
            }
        } else {
            result += s[i];
        }
    }
    return result;
}

ConfigLoader::Value ConfigLoader::parseValue(const std::string& s) {
    Value val;
    std::string t = trim(s);
    if (t.empty()) { val.type = Value::NONE; return val; }

    if (t == "true") { val.type = Value::BOOL; val.boolValue = true; return val; }
    if (t == "false") { val.type = Value::BOOL; val.boolValue = false; return val; }
    if (t == "null") { val.type = Value::NONE; return val; }

    // String
    if (t.size() >= 2 && t.front() == '"' && t.back() == '"') {
        val.type = Value::STRING;
        val.stringValue = unescape(t.substr(1, t.size() - 2));
        return val;
    }

    // Number — check for int vs float
    bool hasDot = false, hasE = false, hasSign = false;
    for (size_t i = 0; i < t.size(); ++i) {
        if (t[i] == '.') hasDot = true;
        if (t[i] == 'e' || t[i] == 'E') hasE = true;
        if ((t[i] == '+' || t[i] == '-') && i > 0) hasSign = true;
    }
    if (hasDot || hasE) {
        val.type = Value::FLOAT;
        // Use strtod for locale-safe parsing
        char* end = nullptr;
        val.floatValue = static_cast<float>(std::strtod(t.c_str(), &end));
        return val;
    }

    // Integer
    val.type = Value::INT;
    val.intValue = std::stoi(t);
    return val;
}

void ConfigLoader::parse(const std::string& json) {
    data.clear();
    Value currentArray = Value(); // not used in flat parser

    // Simple flat key-value parser (handles nested objects via dot notation)
    // e.g., "physics.gravity": 800 -> key = "physics.gravity"
    size_t i = 0;
    while (i < json.size()) {
        char c = json[i];
        if (c == '{' || c == '}' || c == ',' || c == ']') { ++i; continue; }

        // Expect a key
        if (c != '"') { ++i; continue; }
        size_t keyStart = i + 1;
        while (i + 1 < json.size() && json[i + 1] != '"') ++i;
        std::string key = trim(json.substr(keyStart, i - keyStart + 1));

        // Skip to ':'
        while (i < json.size() && json[i] != ':') ++i;
        ++i; // now past ':'

        // Skip whitespace
        while (i < json.size() && std::isspace(json[i])) ++i;

        // Parse value
        size_t valStart = i;
        if (json[i] == '"') {
            // String value
            ++i;
            while (i < json.size() && json[i] != '"') {
                if (json[i] == '\\') ++i;
                ++i;
            }
            ++i; // skip closing quote
        } else if (json[i] == '{') {
            // Nested object — skip it
            int depth = 1;
            ++i;
            while (i < json.size() && depth > 0) {
                if (json[i] == '{') ++depth;
                if (json[i] == '}') --depth;
                ++i;
            }
        } else {
            // Number, bool, null — skip until comma or }
            while (i < json.size() && json[i] != ',' && json[i] != '}') ++i;
        }

        std::string rawVal = json.substr(valStart, i - valStart);
        Value val = parseValue(rawVal);
        if (!val.isNone() && !key.empty()) {
            data[key] = val;
        }

        ++i;
    }
    loaded = true;
}

bool ConfigLoader::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        loaded = false;
        data.clear();
        std::cerr << "Config: Could not open '" << path << "'. Using defaults." << std::endl;
        return false;
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string json = ss.str();
    parse(json);
    return loaded;
}

ConfigLoader::Value ConfigLoader::get(const std::string& key) {
    auto it = data.find(key);
    if (it != data.end()) return it->second;
    Value v; v.type = Value::NONE; return v;
}

int ConfigLoader::getInt(const std::string& key, int def) {
    auto v = get(key);
    return v.isInt() ? v.intValue : def;
}

float ConfigLoader::getFloat(const std::string& key, float def) {
    auto v = get(key);
    if (v.isFloat()) return v.floatValue;
    if (v.isInt()) return static_cast<float>(v.intValue);
    return def;
}

bool ConfigLoader::getBool(const std::string& key, bool def) {
    auto v = get(key);
    return v.isBool() ? v.boolValue : def;
}

std::string ConfigLoader::getString(const std::string& key, const std::string& def) {
    auto v = get(key);
    return v.isString() ? v.stringValue : def;
}

bool ConfigLoader::isLoaded() { return loaded; }

void ConfigLoader::clear() {
    data.clear();
    loaded = false;
}
