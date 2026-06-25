#include "ResourceManager.hpp"

std::unordered_map<std::string, sf::Texture> ResourceManager::textures;
std::unordered_map<std::string, std::pair<sf::Font, unsigned int>> ResourceManager::fonts;
std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> ResourceManager::soundBuffers;
std::unordered_map<std::string, std::unique_ptr<sf::Sound>> ResourceManager::sounds;
std::unordered_map<std::string, std::unique_ptr<sf::Music>> ResourceManager::music;
bool ResourceManager::initialized = false;

void ResourceManager::init() {
    if (!initialized) {
        textures.clear();
        fonts.clear();
        soundBuffers.clear();
        sounds.clear();
        music.clear();
        initialized = true;
    }
}

sf::Texture& ResourceManager::getTexture(const std::string& path) {
    init();
    if (textures.find(path) == textures.end()) {
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            textures[path] = tex;
        } else {
            // Fallback: white placeholder
            sf::Image img({1, 1}, sf::Color::White);
            [[maybe_unused]] auto ok = tex.loadFromImage(img);
            textures[path] = tex;
        }
    }
    return textures[path];
}

const sf::Font& ResourceManager::getFont(const std::string& path, unsigned int size) {
    init();
    std::string key = path + "::" + std::to_string(size);
    if (fonts.find(key) == fonts.end()) {
        sf::Font font;
        if (font.openFromFile(path)) {
            fonts[key] = std::make_pair(std::move(font), size);
        } else {
            // Fallback: default font
            fonts[key] = std::make_pair(sf::Font(), size);
        }
    }
    return fonts[key].first;
}

sf::Sound& ResourceManager::getSound(const std::string& path) {
    init();
    if (sounds.find(path) == sounds.end()) {
        auto buffer = std::make_shared<sf::SoundBuffer>();
        if (buffer->loadFromFile(path)) {
            soundBuffers[path] = buffer;
        } else {
            // Silent fallback: minimal valid WAV (1 sample, 44100 Hz, mono, 16-bit)
            static const unsigned char silentWav[] = {
                'R','I','F','F', 0x24,0x00,0x00,0x00, // RIFF header, size
                'W','A','V','E',                        // fmt chunk type
                0x10,0x00,0x00,0x00,                    // fmt chunk size
                0x01,0x00,                              // PCM format
                0x01,0x00,                              // 1 channel
                0x44,0xAC,0x00,0x00,                    // 44100 Hz
                0x02,0x00,                              // 16-bit
                0x01,0x00,                              // block align
                0x04,0x00,0x00,0x00,                    // data chunk size
                'd','a','t','a',
                0x00,0x00, 0x00,0x00                   // 2 silent samples
            };
            sf::SoundBuffer silentBuf;
            if (silentBuf.loadFromMemory(silentWav, sizeof(silentWav))) {
                soundBuffers[path] = std::make_shared<sf::SoundBuffer>(silentBuf);
            } else {
                soundBuffers[path] = nullptr;
            }
        }
        if (soundBuffers[path]) {
            sounds[path] = std::make_unique<sf::Sound>(*soundBuffers[path]);
        } else {
            sounds[path] = nullptr;
        }
    }
    return *sounds[path];
}

sf::Music* ResourceManager::getMusic(const std::string& path) {
    init();
    if (music.find(path) == music.end()) {
        auto mus = std::make_unique<sf::Music>();
        if (mus->openFromFile(path)) {
            music[path] = std::move(mus);
        } else {
            // Fallback: null pointer or some dummy? 
            // For now, we'll just leave it as nullptr if it fails to load.
            music[path] = nullptr;
        }
    }
    return music[path].get();
}

void ResourceManager::reload(const std::string& path) {
    init();
    for (auto it = textures.begin(); it != textures.end(); ) {
        if (it->first == path) {
            it = textures.erase(it);
        } else {
            ++it;
        }
    }
    for (auto it = fonts.begin(); it != fonts.end(); ) {
        if (it->first.find(path) != std::string::npos) {
            it = fonts.erase(it);
        } else {
            ++it;
        }
    }
    soundBuffers.erase(path);
    sounds.erase(path);
    music.erase(path);
}

void ResourceManager::clear() {
    textures.clear();
    fonts.clear();
    soundBuffers.clear();
    sounds.clear();
    music.clear();
    initialized = false;
}
