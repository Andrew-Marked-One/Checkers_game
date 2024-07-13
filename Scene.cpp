#include "Scene.h"
#include "GameEngine.h"


using ActionMap = std::unordered_map<int, std::string>;

Scene::Scene(GameEngine* gameEngine) {
	m_game = gameEngine;
}

Scene::~Scene() {}

const ActionMap& Scene::getActionMap() const {
	return m_actionMap;
}

bool Scene::hasEnded() const {
	return m_hasEnded;
}

void Scene::registerAction(int inputKey, const std::string& actionName) {
	m_actionMap[inputKey] = actionName;
}

void Scene::resetWindowSize() {
	m_windowSize = m_game->window().getSize();
	m_windowHalfSize = m_windowSize / 2;
}

Entity* Scene::createButton(const std::string& tag, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor) {
	Entity* entity = m_entityManager.addEntity(tag);
	return createButtonHelper(entity, bSize, bPos, bColor, text, font, tColor);
}

Entity* Scene::createButton(const std::vector<std::string>& tags, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor) {
	Entity* entity = m_entityManager.addEntity(tags);
	return createButtonHelper(entity, bSize, bPos, bColor, text, font, tColor);
}

Entity* Scene::createButtonHelper(Entity* button, Vec2 bSize, Vec2 bPos, sf::Color bColor, const std::string& text, const std::string& font, sf::Color tColor) const {
	button->addComponent<CTransform>(bPos);
	auto& buttonSp = button->addComponent<CShape>(bSize, bColor, sf::Color(0, 0, 0), -1).shape;
	buttonSp.setPosition(bPos.x, bPos.y);
	auto& buttonText = button->addComponent<CText>(text, bSize * 0.95, m_game->assets().getFont(font), tColor).text;
	buttonText.setPosition(bPos.x, bPos.y);
	button->addComponent<CFunctions>();
	button->addComponent<CStateUI>();
	return button;
}

Entity* Scene::createText(const std::string& tag, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color) {
	Entity* entity = m_entityManager.addEntity(tag);
	return createTextHelper(entity, text, containerSize, pos, font, color);
}

Entity* Scene::createText(const std::vector<std::string>& tags, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color) {
	Entity* entity = m_entityManager.addEntity(tags);
	return createTextHelper(entity, text, containerSize, pos, font, color);
}

Entity* Scene::createTextHelper(Entity* textEntity, const std::string& text, Vec2 containerSize, Vec2 pos, const std::string& font, sf::Color color) const {
	textEntity->addComponent<CTransform>(pos);
	auto& textEntityText = textEntity->addComponent<CText>(text, containerSize, m_game->assets().getFont(font), color).text;
	textEntityText.setPosition(pos.x, pos.y);
	return textEntity;
}

void Scene::resizeText(const std::string& textEntityTag) {
	auto& entities = m_entityManager.getEntities(textEntityTag);
	if (entities.empty()) {
		std::cerr << "Scene::resizeText: no entity with tag \"" << textEntityTag << "\" found\n";
		return;
	}

	auto [minContainer, maxText] = getMinContainerMaxText(entities);

	Vec2 maxTextScaled = maxText * (minContainer / maxText).min();

	for (Entity* entity : entities) {
		if (!entity->hasComponent<CText>()) {
			continue;
		}

		auto& entityCText = entity->getComponent<CText>();
		entityCText.setScale(maxTextScaled, true);
	}
}

void Scene::resizeText(const std::string& textEntityTag, unsigned int charSize) {
	auto& entities = m_entityManager.getEntities(textEntityTag);
	if (entities.empty()) {
		std::cerr << "Scene::resizeText: no entity with tag '" << textEntityTag << "' found\n";
		return;
	}

	for (Entity* entity : entities) {
		if (!entity->hasComponent<CText>()) {
			continue;
		}

		auto& entityCText = entity->getComponent<CText>();
		entityCText.text.setCharacterSize(100);
		entityCText.centerText();

		float scale = charSize / 100.f;
		entityCText.text.setScale(scale, scale);
	}
}

std::pair<Vec2, Vec2> Scene::getMinContainerMaxText(const std::vector<Entity*>& entities) const {
	Vec2 minContainer = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	Vec2 maxText = { 0, 0 };

	for (Entity* entity : entities) {
		if (!entity->hasComponent<CText>()) {
			continue;
		}

		auto& entityText = entity->getComponent<CText>();
		Vec2 containerSize = entityText.containerSize;
		Vec2 textSize = entityText.text.getLocalBounds().getSize();

		minContainer.x = std::min(minContainer.x, containerSize.x);
		minContainer.y = std::min(minContainer.y, containerSize.y);

		maxText.x = std::max(maxText.x, textSize.x);
		maxText.y = std::max(maxText.y, textSize.y);
	}

	return { minContainer, maxText };
}

void Scene::buttonHover() {
	for (Entity* button : m_entityManager.getEntities("button")) {
		auto& buttonS = button->getComponent<CStateUI>();
		auto& buttonSh = button->getComponent<CShape>().shape;
		bool isInside = isMouseInside(button);
		if (isInside && !buttonS.isHovering) {
			sf::Color tempColor = buttonSh.getFillColor();
			buttonSh.setFillColor(tempColor + sf::Color(50, 50, 50, 0));
			buttonS.isHovering = true;
		}
		else if (!isInside && buttonS.isHovering) {
			sf::Color tempColor = buttonSh.getFillColor();
			buttonSh.setFillColor(tempColor - sf::Color(50, 50, 50, 0));
			buttonS.isHovering = false;
		}
	}
}

void Scene::buttonLeftClick() {
	for (Entity* button : m_entityManager.getEntities("button")) {
		if (button->isHiding() || !isMouseInside(button)) {
			continue;
		}

		button->getComponent<CFunctions>().invokeFunc("leftClick");
		break;
	}
}

bool Scene::isMouseInside(const Entity* entity) const {
	auto getEntityHalfSize = [](const Entity* entity) {
		if (entity->hasComponent<CShape>()) {
			return entity->getComponent<CShape>().shape.getGlobalBounds().getSize() / 2.f;
		}
		else {
			return entity->getComponent<CAnimation>().animation.getSprite().getGlobalBounds().getSize() / 2.f;
		}
	};

	Vec2 eHalfSize = getEntityHalfSize(entity);
	Vec2 ePos = entity->getComponent<CTransform>().pos;
	Vec2 delta = (m_mousePos - ePos).abs();

	return delta.x < eHalfSize.x && delta.y < eHalfSize.y;
}

void Scene::drawEntities() {
	for (Entity* entity : m_entityManager.getEntities()) {
		if (entity->isHiding()) {
			continue;
		}

		drawEntitiesHelper(entity);
	}
}

void Scene::drawEntitiesHelper(Entity* entity) {
	Vec2 entityPos = entity->getComponent<CTransform>().pos;
	if (entity->hasComponent<CAnimation>()) {
		auto& entitySp = entity->getComponent<CAnimation>().animation.getSprite();
		entitySp.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entitySp);
	}
	else if (entity->hasComponent<CShape>()) {
		auto& entitySh = entity->getComponent<CShape>().shape;
		entitySh.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entitySh);
	}

	if (entity->hasComponent<CText>()) {
		auto& entityText = entity->getComponent<CText>().text;
		entityText.setPosition(entityPos.x, entityPos.y);
		m_game->window().draw(entityText);
	}
}

void Scene::drawEntities(const std::string& tag) {
	for (Entity* entity : m_entityManager.getEntities(tag)) {
		if (entity->isHiding()) {
			continue;
		}

		drawEntitiesHelper(entity);
	}
}

void Scene::drawEntitiesExcept(const std::string& tag) {
	for (Entity* entity : m_entityManager.getEntities()) {
		if (entity->isHiding() || entity->hasTag(tag)) {
			continue;
		}

		drawEntitiesHelper(entity);
	}
}

void Scene::hideEntities(const std::string& tag) {
	auto& entities = m_entityManager.getEntities(tag);
	if (entities.empty()) {
		std::cerr << "Scene::hideEntities: no entity with tag \"" << tag << "\" found\n";
		return;
	}

	for (Entity* entity : entities) {
		entity->hide();
	}
}

void Scene::revealEntities(const std::string& tag) {
	auto& entities = m_entityManager.getEntities(tag);
	if (entities.empty()) {
		std::cerr << "Scene::revealEntities: no entity with tag \"" << tag << "\" found\n";
		return;
	}

	for (Entity* entity : entities) {
		entity->reveal();
	}
}

void Scene::resizeEntities(float relDiff, float prevOffsetX, float newOffsetX) {
	for (Entity* entity : m_entityManager.getEntities()) {
		auto& entityPos = entity->getComponent<CTransform>().pos;
		entityPos = { (entityPos.x - prevOffsetX) * relDiff + newOffsetX, entityPos.y * relDiff };

		if (entity->hasComponent<CAnimation>()) {
			auto& entitySp = entity->getComponent<CAnimation>().animation.getSprite();
			entitySp.setScale(entitySp.getScale() * relDiff);
		}
		else if (entity->hasComponent<CShape>()) {
			auto& entitySh = entity->getComponent<CShape>().shape;
			entitySh.setSize(entitySh.getSize() * relDiff);
			entitySh.setOrigin(entitySh.getSize() / 2.f);
		}

		if (entity->hasComponent<CText>()) {
			auto& entityText = entity->getComponent<CText>();
			entityText.text.setScale(entityText.text.getScale() * relDiff);
			entityText.containerSize *= relDiff;
		}
	}
}

void Scene::resetBackgound() {
	Entity* background = m_entityManager.getEntities("background")[0];
	background->getComponent<CTransform>().pos = m_windowHalfSize;
	background->getComponent<CAnimation>().animation.setScale(m_windowSize, false);
}