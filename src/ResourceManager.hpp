#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class ResourceManager {
public:
    static sf::Texture& getTexture(const std::string& path);
    static sf::Font     getFont(const std::string& path, unsigned int size);
    static sf::Sound    getSound(const std::string& path);

    static void reload(const std::string& path);
    static void clear();

private:
    static std::unordered_map<std::string, sf::Texture> textures;
    static std::unordered_map<std::string, std::pair<sf::Font, unsigned int>> fonts;
    static std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> soundBuffers;
    static bool initialized;

    static void init();
};

#endif // RESOURCE_MANAGER_HPP
