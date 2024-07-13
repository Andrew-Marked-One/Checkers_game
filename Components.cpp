#include "Components.h"


CTransform::CTransform() {}

CTransform::CTransform(Vec2 Pos)
	: pos{ Pos } {}


CAnimation::CAnimation() {}

CAnimation::CAnimation(const Animation& animation_, bool isRepeating_)
	: animation{ animation_ }
	, isRepeating{ isRepeating_ } {}


CShape::CShape() {}

CShape::CShape(Vec2 size, sf::Color fill, sf::Color outline, float thickness)
	: shape{ sf::Vector2f(size.x, size.y) } {

	shape.setFillColor(fill);
	shape.setOutlineColor(outline);
	shape.setOutlineThickness(thickness);
	shape.setOrigin(size.x / 2, size.y / 2);
}

void CShape::setScale(Vec2 relVal, bool maintainRatio) {
	Vec2 shapeSize = shape.getLocalBounds().getSize();

	if (maintainRatio) {
		float realScale = (relVal / shapeSize).min();
		shape.setScale(realScale, realScale);
	}
	else {
		Vec2 realScale = relVal / shapeSize;
		shape.setScale(realScale.x, realScale.y);
	}
}

CStateCell::CStateCell() {}


CStateUI::CStateUI() {}


CText::CText() {}

CText::CText(const std::string& textStr, Vec2 containerSize_, const sf::Font& font, sf::Color color)
	: containerSize{ containerSize_ } {

	text.setString(textStr);
	text.setFont(font);
	text.setCharacterSize(100);
	text.setFillColor(color);

	setScale(containerSize, true);
	centerText();
}

void CText::setScale(Vec2 relVal, bool maintainRatio) {
	Vec2 textSize = text.getLocalBounds().getSize();

	if (maintainRatio) {
		float realScale = (relVal / textSize).min();
		text.setScale(realScale, realScale);
	}
	else {
		Vec2 realScale = relVal / textSize;
		text.setScale(realScale.x, realScale.y);
	}
}

void CText::centerText() {
	sf::FloatRect textSizeRect = text.getLocalBounds();
	Vec2 textSize = textSizeRect.getSize();
	text.setOrigin(textSizeRect.left + textSize.x / 2, textSizeRect.top + textSize.y / 2);
}

CFunctions::CFunctions() {}

void CFunctions::addFunc(const std::string& funcName, const std::function<void()>& func) {
	m_functions[funcName] = func;
}

void CFunctions::invokeFunc(const std::string& funcName) const {
	std::invoke(m_functions.at(funcName));
}