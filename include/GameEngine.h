#pragma once
#include "Scene.h"
#include "Assets.h"
#include <thread>


class GameEngine {
	class SceneChangeArgs {
	public:
		std::string sceneName = "NONE";
		std::unique_ptr<Scene> scene = nullptr;
		bool endCurScene = false;

		SceneChangeArgs() {}
		SceneChangeArgs(const std::string& sceneName_, std::unique_ptr<Scene>&& scene_, bool endCurScene_)
			: sceneName{ sceneName_ }
			, scene{ std::move(scene_) }
			, endCurScene{ endCurScene_ } {}
	};

public:
	GameEngine(const std::string& configPath);

	void changeScene(const std::string& sceneName, std::unique_ptr<Scene>&& scene, bool endCurScene);
	void run();
	bool isRunning() const;
	void quit();
	sf::RenderWindow& window();
	int getFramerateLimit() const;
	const Assets& assets() const;
	const std::string& getConfigPath() const;

private:
	sf::RenderWindow m_window;
	Assets m_assets;
	std::unordered_map<std::string, std::unique_ptr<Scene>> m_sceneMap;
	SceneChangeArgs m_nextSceneChangeArgs;
	std::string m_curSceneName = "NONE";
	std::string m_configPath = "NONE";
	int m_framerateLimit;
	bool m_isRunning = true;

	void init();
	void loadFromConfig();
	Scene* currentScene();
	void sUserInput();
	void delayedSceneChange();
};
