#pragma once
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Animation.h"


class Assets {
public:
	void loadAssets(const std::string& path);
	void loadTextures(const std::string& name, const std::string& path);
	void loadAnimation(const std::string& name, const std::string& textureName, int frames, int duration);
	void loadFont(const std::string& name, const std::string& path);
	const Animation& getAnimation(const std::string& name) const;
	const sf::Font& getFont(const std::string& name) const;

private:
	std::unordered_map<std::string, sf::Font>                   m_fontMap;
	std::unordered_map<std::string, sf::Texture>                m_textureMap;
	std::unordered_map<std::string, std::unique_ptr<Animation>> m_animationMap;
};
