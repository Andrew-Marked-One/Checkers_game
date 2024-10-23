#include "Scene_Menu.h"
#include "Scene_Play.h"


#pragma region Initialization
Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene{ gameEngine } {

	init();
}

void Scene_Menu::init() {
	registerAction(sf::Keyboard::Escape, "QUIT");

	resetWindowSize();

	initBackground();
	initTitle();
	initButtons();

	m_entityManager.update();

	hideEntities("playerVsBotOption");
	hideEntities("playOption");
	hideEntities("settings");

	resizeText("button");
}

void Scene_Menu::initBackground() {
	Entity* background = m_entityManager.addEntity("background");
	background->addComponent<CTransform>(m_windowHalfSize);
	auto& animation = m_game->assets().getAnimation("background");
	auto& backgroundA = background->addComponent<CAnimation>(animation, true).animation;
	backgroundA.setScale(m_windowSize, false);
}

void Scene_Menu::initTitle() {
	float smallestSide = m_windowSize.min();
	Vec2 textPos = { m_windowHalfSize.x, smallestSide * 0.15f };
	Vec2 container = { smallestSide, smallestSide };

	Entity* title = createText("title", "Checkers Game 3000", container, textPos, "Marhey", sf::Color(141, 141, 141));
	auto& titleText = title->getComponent<CText>().text;
	titleText.setOutlineColor(sf::Color(0, 0, 0));
	titleText.setOutlineThickness(-2.f);
}

void Scene_Menu::initButtons() {
	float smallestSide = m_windowSize.min();
	Vec2 buttonSize = m_relButtonSize * smallestSide;
	Vec2 buttonPos = { m_windowHalfSize.x, smallestSide * 0.4f };
	float offsetY = m_relButtonDist * buttonSize.y;

	initMenuButtons(buttonSize, buttonPos, offsetY);
	initPlayButtons(buttonSize, buttonPos, offsetY);
	initPlayerVsBotButtons(buttonSize, buttonPos, offsetY);
	initSettings(buttonSize, buttonPos, offsetY);
}

void Scene_Menu::initMenuButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY) {
	std::vector<std::string> tags = { "button", "menuOption" };

	Entity* buttonPlay = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Play", "Marhey", m_buttonTextColor);
	buttonPlay->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("menuOption");
		revealEntities("playOption");
	});

	buttonPos.y += offsetY;
	Entity* buttonSettings = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Settings", "Marhey", m_buttonTextColor);
	buttonSettings->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("menuOption");
		revealEntities("settings");
	});

	buttonPos.y += offsetY;
	Entity* buttonExit = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Exit", "Marhey", m_buttonTextColor);
	buttonExit->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_hasEnded = true;
	});
}

void Scene_Menu::initPlayButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY) {
	std::vector<std::string> tags = { "button", "playOption" };

	Entity* buttonPvsP = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Player vs Player", "Marhey", m_buttonTextColor);
	buttonPvsP->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, false, false), true);
	});

	buttonPos.y += offsetY;
	Entity* buttonPvsB = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Player vs Bot", "Marhey", m_buttonTextColor);
	buttonPvsB->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("playOption");
		revealEntities("playerVsBotOption");
	});

	buttonPos.y += offsetY;
	Entity* buttonBvsB = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Bot vs Bot", "Marhey", m_buttonTextColor);
	buttonBvsB->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, true, true), true);
	});

	buttonPos.y += offsetY;
	Entity* buttonBack = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Back", "Marhey", m_buttonTextColor);
	buttonBack->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("playOption");
		revealEntities("menuOption");
	});
}

void Scene_Menu::initPlayerVsBotButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY) {
	std::vector<std::string> tags = { "button", "playerVsBotOption" };
	float offsetX = (buttonSize.x + (offsetY - buttonSize.y)) / 2;

	buttonPos += { -offsetX, offsetY };
	Entity* buttonPlayWhite = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Play white pieces", "Marhey", m_buttonTextColor);
	buttonPlayWhite->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, false, true), true);
	});

	buttonPos.x += offsetX * 2;
	Entity* buttonPlayBlack = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Play black pieces", "Marhey", m_buttonTextColor);
	buttonPlayBlack->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, true, false), true);
	});

	buttonPos += Vec2(-offsetX, offsetY);
	Entity* buttonBack = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Back", "Marhey", m_buttonTextColor);
	buttonBack->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("playerVsBotOption");
		revealEntities("playOption");
	});
}

void Scene_Menu::initSettings(Vec2 buttonSize, Vec2 buttonPos, float offsetY) {
	Vec2 backShapeSize = { buttonSize.x * 1.5, (buttonSize.y * 3 + (offsetY - buttonSize.y) * 2) * 1.5 };
	Vec2 backShapePos = { buttonPos.x, buttonPos.y + offsetY };

	Entity* backShape = m_entityManager.addEntity("settings");
	backShape->addComponent<CTransform>(backShapePos);
	backShape->addComponent<CShape>(backShapeSize, sf::Color(50, 50, 50), sf::Color(0, 0, 0), -1);

	Vec2 container = { backShapeSize.x * 0.9, buttonSize.y };
	Entity* textEntity = createText("settings", "Go to bin/config.txt :)", container, buttonPos, "Marhey", sf::Color(141, 141, 141));
	auto& textEntityText = textEntity->getComponent<CText>().text;
	textEntityText.setOutlineColor(sf::Color(0, 0, 0));
	textEntityText.setOutlineThickness(-2.f);

	buttonPos.y += offsetY * 2;
	Entity* buttonBack = createButton(std::vector<std::string>{ "button", "settings" }, buttonSize, buttonPos , m_buttonColor, "Back", "Marhey", m_buttonTextColor);
	buttonBack->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		hideEntities("settings");
		revealEntities("menuOption");
	});
}

void Scene_Menu::resizeGame() {
	Vec2 prevWinSize = m_windowSize;

	resetWindowSize();

	float relDiff = m_windowSize.min() / prevWinSize.min();
	float prevOffsetX = (prevWinSize.x - prevWinSize.min()) / 2;
	float newOffsetX = (m_windowSize.x - m_windowSize.min()) / 2;

	resizeEntities(relDiff, prevOffsetX, newOffsetX);
	resetBackgound();
}
#pragma endregion

void Scene_Menu::update() {
	m_currentFrame++;
}

void Scene_Menu::onEnd() {
	m_game->quit();
}

void Scene_Menu::pause() {
	m_isPaused = !m_isPaused;
}

void Scene_Menu::sDoAction(const Action& action){
	if (action.type() == "START") {
		if (action.name() == "QUIT") {
			m_hasEnded = true;
		}
		else if (action.name() == "MOUSE_MOVE") {
			m_mousePos = action.pos();
			buttonHover();
		}
		else if (action.name() == "LEFT_CLICK") {
			buttonLeftClick();
		}
		//else if (action.name() == "RIGHT_CLICK") {}
		//else if (action.name() == "MIDDLE_CLICK") {}
		else if (action.name() == "RESIZED") {
			resizeGame();
		}
	}
	//else if (action.type() == "END") {
		//if (action.name() == "LEFT_CLICK") {}
		//else if (action.name() == "RIGHT_CLICK") {}
		//else if (action.name() == "MIDDLE_CLICK") {}
	//}
}

void Scene_Menu::sRender() {
	m_game->window().clear();

	drawEntities();

	m_game->window().display();
}
