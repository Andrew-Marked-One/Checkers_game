#include "Animation.h"


Animation::Animation() {}

Animation::Animation(const std::string& name, const sf::Texture& texture, int frameCount, int speed)
	: m_name{ name }
	, m_sprite{ texture }
	, m_frameCount{ frameCount }
	, m_speed{ speed } {

	m_tSize = texture.getSize();
	m_sprite.setOrigin(m_tSize.x / 2, m_tSize.y / 2);
	m_sprite.setTextureRect(sf::IntRect(m_currentFrame * m_tSize.x, 0, m_tSize.x, m_tSize.y));
}

void Animation::update() {
	if (m_speed > 0) {
		m_sprite.setTextureRect(sf::IntRect((m_currentFrame / m_speed) % m_frameCount * m_tSize.x, 0, m_tSize.x, m_tSize.y));
	}
	m_currentFrame++;
}

Vec2 Animation::getTextureSize() const {
	return m_tSize;
}

const std::string& Animation::getName() const {
	return m_name;
}

const sf::Sprite& Animation::getSprite() const {
	return m_sprite;
}

sf::Sprite& Animation::getSprite() {
	return m_sprite;
}

bool Animation::hasEnded() const {
	return floor(m_currentFrame / m_speed) >= m_frameCount;
}

void Animation::setScale(Vec2 relVal, bool maintainRatio) {
	Vec2 spriteSize = m_sprite.getLocalBounds().getSize();

	if (maintainRatio) {
		float realScale = (relVal / spriteSize).min();
		m_sprite.setScale(realScale, realScale);
	}
	else {
		Vec2 realScale = relVal / spriteSize;
		m_sprite.setScale(realScale.x, realScale.y);
	}
}

void Animation::changeDirecHor() {
	Vec2 scale = m_sprite.getScale();
	m_sprite.scale(-scale.x, scale.y);
}



