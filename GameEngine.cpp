#include "GameEngine.h"
#include "Scene_Menu.h"


GameEngine::GameEngine(const std::string& configPath)
	: m_configPath{ configPath } {

	init();
}

void GameEngine::init() {
	loadFromConfig();
	changeScene("MENU", std::make_unique<Scene_Menu>(this), true);
}

void GameEngine::loadFromConfig() {
	std::ifstream file(m_configPath);
	if (!file.is_open()) {
		std::cerr << "GameEngine::loadFromConfig: cannot open file with path \"" << m_configPath << "\"\n";
	}
	std::string line;
	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type;
		iss >> type;
		if (type == "Window") {
			int wWidth, wHeight;
			iss >> wWidth >> wHeight >> m_framerateLimit;
			m_window.create(sf::VideoMode(wWidth, wHeight), "Checkers");
			m_window.setFramerateLimit(m_framerateLimit);
		}
		else if (type == "Assets") {
			std::string assetsPath;
			iss >> assetsPath;
			m_assets.loadAssets(assetsPath);
			break;
		}
	}
}

void GameEngine::changeScene(const std::string& sceneName, std::unique_ptr<Scene>&& scene, bool endCurScene) {
	m_nextSceneChangeArgs = { sceneName, std::move(scene), endCurScene };
}

void GameEngine::run() {
	while (isRunning()) {
		try {
			if (m_nextSceneChangeArgs.scene) {
				delayedSceneChange();
			}

			sUserInput();

			if (currentScene()->hasEnded()) {
				currentScene()->onEnd();
			}

			currentScene()->update();
			currentScene()->sRender();
		}
		catch (const std::exception& e) {
			std::cerr << "GameEngine::run, exception caught: " << e.what() << '\n';
			quit();
		}
	}
}

bool GameEngine::isRunning() const {
	return m_isRunning && m_window.isOpen();
}

Scene* GameEngine::currentScene() {
	return m_sceneMap[m_curSceneName].get();
}

void GameEngine::delayedSceneChange() {
	if (m_nextSceneChangeArgs.endCurScene) {
		m_sceneMap.erase(m_curSceneName);
	}
	m_sceneMap[m_nextSceneChangeArgs.sceneName] = std::move(m_nextSceneChangeArgs.scene);
	m_curSceneName = std::move(m_nextSceneChangeArgs.sceneName);
	m_nextSceneChangeArgs.scene = nullptr;
}

void GameEngine::quit() {
	m_isRunning = false;
}

void GameEngine::sUserInput() {
	sf::Event event;
	while (m_window.pollEvent(event)) {
		if (event.type == sf::Event::Closed) {
			quit();
		}
		else if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) {
			if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end()) {
				continue;
			}
			std::string actionType = event.type == sf::Event::KeyPressed ? "START" : "END";
			currentScene()->sDoAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}
		else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
			Vec2 mpos = sf::Mouse::getPosition(m_window);
			std::string actionType = (event.type == sf::Event::MouseButtonPressed) ? "START" : "END";
			switch (event.mouseButton.button) {
			case sf::Mouse::Left:
				currentScene()->sDoAction(Action("LEFT_CLICK", actionType, mpos));
				break;
			case sf::Mouse::Right:
				currentScene()->sDoAction(Action("RIGHT_CLICK", actionType, mpos));
				break;
			case sf::Mouse::Middle:
				currentScene()->sDoAction(Action("MIDDLE_CLICK", actionType, mpos));
				break;
			}

		}
		else if (event.type == sf::Event::MouseMoved) {
			currentScene()->sDoAction(Action("MOUSE_MOVE", "START", Vec2(sf::Mouse::getPosition(m_window))));
		}
		else if (event.type == sf::Event::Resized) {
			sf::Vector2u newWinSize = { std::max(event.size.width, 120u), std::max(event.size.height, 120u) };
			m_window.setSize(newWinSize);

			sf::FloatRect visibleArea = { 0, 0, static_cast<float>(newWinSize.x), static_cast<float>(newWinSize.y) };
			m_window.setView(sf::View(visibleArea));

			currentScene()->sDoAction(Action("RESIZED", "START"));
		}
	}
}

sf::RenderWindow& GameEngine::window() {
	return m_window;
}

int GameEngine::getFramerateLimit() const {
	return m_framerateLimit;
}

const Assets& GameEngine::assets() const {
	return m_assets;
}

const std::string& GameEngine::getConfigPath() const {
	return m_configPath;
}
