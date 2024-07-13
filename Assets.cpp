#include "Assets.h"


void Assets::loadAssets(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Assets::loadAssets: cannot open file with path \"" << path << "\"\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type, name, path;
		iss >> type;
		if (type == "Texture") {
			iss >> name >> path;
			loadTextures(name, path);
		}
		else if (type == "Animation") {
			std::string textName;
			unsigned int frames, duration;
			iss >> name >> textName >> frames >> duration;
			loadAnimation(name, textName, frames, duration);
		}
		else if (type == "Font") {
			iss >> name >> path;
			loadFont(name, path);
		}
	}
}

void Assets::loadTextures(const std::string& name, const std::string& path) {
	sf::Texture texture;
	if (texture.loadFromFile(path)) {
		m_textureMap[name] = texture;
	}
	else {
		std::cerr << "Assets::loadTextures: cannot open file with path \"" << path << "\"\n";
	}
}

void Assets::loadAnimation(const std::string& name, const std::string& textureName, int frames, int duration) {
	m_animationMap[name] = std::make_unique<Animation>(name, m_textureMap[textureName], frames, duration);
}

void Assets::loadFont(const std::string& name, const std::string& path) {
	sf::Font font;
	if (font.loadFromFile(path)) {
		m_fontMap[name] = font;
	}
	else {
		std::cerr << "Assets::loadFont: cannot open file with path \"" << path << "\"\n";
	}
}

const Animation& Assets::getAnimation(const std::string& name) const {
	return *m_animationMap.at(name);
}

const sf::Font& Assets::getFont(const std::string& name) const {
	return m_fontMap.at(name);
}

