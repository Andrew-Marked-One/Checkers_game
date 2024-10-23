#pragma once
#include "GameEngine.h"


class Scene_Menu : public Scene {
public:
	Scene_Menu(GameEngine* gameEngine);

private:
	Vec2 m_relButtonSize = { 0.4f, 0.1f };
	float m_relButtonDist = 1.25f;
	sf::Color m_buttonColor = { 141, 141, 141 };
	sf::Color m_buttonTextColor = { 0, 0, 0 };

	void init();
	void initBackground();
	void initTitle();
	void initButtons();
	void initMenuButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY);
	void initPlayButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY);
	void initPlayerVsBotButtons(Vec2 buttonSize, Vec2 buttonPos, float offsetY);
	void initSettings(Vec2 buttonSize, Vec2 buttonPos, float offsetY);
	void resizeGame();
	void update() override;
	void onEnd() override;
	void pause() override;
	void sDoAction(const Action& action) override;
	void sRender() override;
};
