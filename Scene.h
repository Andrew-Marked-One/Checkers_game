#pragma once
#include "EntityManager.h"
#include "Action.h"


class GameEngine;

class Scene {
	using ActionMap = std::unordered_map<int, std::string>;

public:
	Scene(GameEngine* gameEngine);
	virtual ~Scene();

	virtual void update() = 0;
	virtual void onEnd() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;
	const ActionMap& getActionMap() const;
	bool hasEnded() const;

protected:
	EntityManager m_entityManager;
	ActionMap m_actionMap;
	GameEngine* m_game = nullptr;
	Vec2 m_mousePos;
	Vec2 m_windowSize;
	Vec2 m_windowHalfSize;
	int m_currentFrame = 0;
	bool m_isPaused = false;
	bool m_hasEnded = false;

	virtual void pause() = 0;
	void registerAction(int inputKey, const std::string& actionName);
	void resetWindowSize();
	Entity* createButton(const std::string& tag, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor);
	Entity* createButton(const std::vector<std::string>& tags, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor);
	Entity* createText(const std::string& tags, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color);
	Entity* createText(const std::vector<std::string>& tags, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color);
	void resizeText(const std::string& textEntityTag);
	void resizeText(const std::string& textEntityTag, unsigned int charSize);
	void buttonHover();
	void buttonLeftClick();
	bool isMouseInside(const Entity* entity) const;
	void drawEntities();
	void drawEntities(const std::string& tag);
	void drawEntitiesExcept(const std::string& tag);
	void hideEntities(const std::string& tag);
	void revealEntities(const std::string& tag);
	void resizeEntities(float relDiff, float prevOffsetX, float newOffsetX);
	void resetBackgound();

private:
	Entity* createButtonHelper(Entity* buttone, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor) const;
	Entity* createTextHelper(Entity* textEntity, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color) const;
	std::pair<Vec2, Vec2> getMinContainerMaxText(const std::vector<Entity*>& entities) const;
	void drawEntitiesHelper(Entity* entity);
};