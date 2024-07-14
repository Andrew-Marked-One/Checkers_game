#pragma once
#include <functional>
#include <iostream>
#include "Animation.h"


class CTransform {
public:
	Vec2 pos      = { 0, 0 };
	Vec2 velocity = { 0, 0 };
	Vec2 target   = { 0, 0 };

	CTransform();
	CTransform(Vec2 Pos);
};

class CAnimation {
public:
	Animation animation;
	bool isRepeating = false;

	CAnimation();
	CAnimation(const Animation& animation_, bool isRepeating_);
};

class CShape {
public:
	sf::RectangleShape shape;

	CShape();
	CShape(Vec2 size, sf::Color fill, sf::Color outline, float thickness);

	void setScale(Vec2 relVal, bool maintainRatio);
};

class CStateCell {
public:
	bool isWhite           = false;
	bool isKing            = false;
	bool isOccupied        = false;
	bool isOccupiedByWhite = false;
	bool canMove           = true;
	bool canJump           = true;
	bool hasJumped         = false;
	bool hasToJump         = false;

	CStateCell();
};

class CStateUI {
public:
	bool isHovering = false;

	CStateUI();
};

class CText {
public:
	sf::Text text;
	Vec2 containerSize = { 0, 0 };
	CText();
	CText(const std::string& textStr, Vec2 containerSize_, const sf::Font& font, sf::Color color);

	void setScale(Vec2 relVal, bool maintainRatio);
	void centerText();
};

class CFunctions {
public:
	CFunctions();

	void addFunc(const std::string& funcName, const std::function<void()>& func);
	void invokeFunc(const std::string& funcName) const;

private:
	std::unordered_map<std::string, std::function<void()>> m_functions;
};
