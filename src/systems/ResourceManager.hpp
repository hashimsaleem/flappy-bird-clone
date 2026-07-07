#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

class ResourceManager {
public:
    static ResourceManager& getInstance() {
        static ResourceManager instance;
        return instance;
    }

    sf::Texture& getTexture(const std::string& path);
    const sf::Font& getFont(const std::string& path, unsigned int size);
    sf::Sound&     getSound(const std::string& path);
    sf::Music*     getMusic(const std::string& path);

    void reload(const std::string& path);
    void clear();

private:
    ResourceManager() : initialized(false) {}
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    void init();

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_map<std::string, std::pair<sf::Font, unsigned int>> fonts;
    std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Sound>> sounds;
    std::unordered_map<std::string, std::unique_ptr<sf::Music>> music;
    bool initialized;
};

#endif // RESOURCE_MANAGER_HPP
