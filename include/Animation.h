#pragma once
#include "Vec2.h"


class Animation {
public:
	Animation();
	Animation(const std::string& name, const sf::Texture& texture, int frameCount, int speed);

	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	Vec2 getTextureSize() const;
	const sf::Sprite& getSprite() const;
	sf::Sprite& getSprite();
	void setScale(Vec2 relVal, bool maintainRatio);
	void changeDirecHor();

private:
	sf::Sprite m_sprite;
	std::string m_name = "NONE";
	Vec2 m_tSize  = { 1, 1 };
	int m_frameCount   = 1;
	int m_currentFrame = 0;
	int m_speed        = 0;
};
