#pragma once
#include <thread>
#include <mutex>
#include <array>
#include "GameEngine.h"
#include "Timer.h"
#include "CircularBuffer.h"


#pragma region HelperClasses
class Path {
public:
	std::vector<int> actionIndices;
	std::vector<int> idVec;
	int amountOfJumps = 0;

	void clear() {
		idVec.clear();
		actionIndices.clear();
		amountOfJumps = 0;
	}

	void addActionAndId(int action, int cellId) {
		actionIndices.emplace_back(action);
		idVec.emplace_back(cellId);
	}
};

class CheckerAmount {
public:
	int whiteCheckersAll = 12;
	int blackCheckersAll = 12;
	int whiteCheckersKing = 0;
	int blackCheckersKing = 0;
};

class GameStateHist {
public:
	std::array<std::array<CStateCell, 8>, 8> allCellsState;
	std::array<std::array<Entity*, 8>, 8> cellToChecker = {};
	CheckerAmount checkerAmount;
	bool isWhitesTurn = true;

	GameStateHist()
		: cellToChecker{} {}

	GameStateHist(const std::array<std::array<CStateCell, 8>, 8>& allCellsState_, const std::array<std::array<Entity*, 8>, 8>& cellToChecker_, CheckerAmount checkerAmount_, bool isWhitesTurn_)
		: allCellsState{ allCellsState_ }
		, cellToChecker{ cellToChecker_ }
		, checkerAmount{ checkerAmount_ }
		, isWhitesTurn{ isWhitesTurn_ } {}
};

class GameState {
public:
	std::unordered_map<int, std::vector<int>> allEndCells;
	std::unordered_map<int, std::unordered_map<int, std::vector<Path>>> allPaths;
	std::array<std::array<CStateCell, 8>, 8> allCellsState;
	std::vector<Entity*> canMoveCells = {};
	Path path;
	Entity* selectedCell = nullptr;
	CheckerAmount checkerAmount;
	bool isWhitesTurn = true;

	GameState() {}
};

class AllPathsSearchResult {
public:
	std::unordered_map<int, std::vector<int>> allEndCells;
	std::unordered_map<int, std::unordered_map<int, std::vector<Path>>> allPaths;
	std::vector<Entity*> canMoveCells = {};

	template<typename T1, typename T2, typename T3>
	AllPathsSearchResult(T1&& allEndCells_, T2&& allPaths_, T3&& canMoveCells_)
		: allEndCells{ std::forward<T1>(allEndCells_) }
		, allPaths{ std::forward<T2>(allPaths_) }
		, canMoveCells{ std::forward<T3>(canMoveCells_) } {}
};

class ThreadTask {
public:
	GameState gameState;
	int canMoveIndex = 0;
	int endIndex = 0;
	int taskId = 0;
	ThreadTask() {}
	ThreadTask(const GameState& gameState_, int canMoveIndex_, int endIndex_, int taskId_)
		: gameState{ gameState_ }
		, canMoveIndex{ canMoveIndex_ }
		, endIndex{ endIndex_ }
		, taskId{ taskId_ } {}
};

class EvaluatedMove {
public:
	int value = 0;
	int canMoveIndex = 0;
	int endIndex = 0;

	EvaluatedMove() {};
	EvaluatedMove(int value_, int canMoveIndex_, int endIndex_)
		: value{ value_ }
		, canMoveIndex{ canMoveIndex_ }
		, endIndex{ endIndex_ } {}
};

#pragma endregion


class Scene_Play : public Scene {

public:
	Scene_Play(GameEngine* gameEngine, bool isWhitePlayerBot, bool isBlackPlayerBot);
	~Scene_Play();

private:
#pragma region MemberVariables
	GameState m_gameState;
	CircularBuffer<GameStateHist> m_gameHistory;
	Timer m_mainTimer;
	Timer m_searchTimer;
	std::jthread m_searchThread;

	//up-left, up-right, down-left, down-right
	std::array<sf::Vector2i, 4> m_actions = { sf::Vector2i(-1, -1), sf::Vector2i(1, -1), sf::Vector2i(-1, 1), sf::Vector2i(1, 1) };
	std::array<std::array<Entity*, 8>, 8> m_cells = {};
	std::array<std::array<Entity*, 8>, 8> m_cellToChecker = {};

	sf::FloatRect m_gridBounds;
	Vec2 m_gridSize = { 0, 0 };
	Vec2 m_gridHalfSize = { 0, 0 };

	float m_movementSpeed;
	float m_movementSpeedScaled;

	int m_searchDepthLimitWhite;
	int m_searchDepthLimitBlack;
	int m_currentSearchDepthLimit;
	int m_bestCanMoveIndex;
	int m_bestEndIndex;

	sf::Color m_cellColorW;
	sf::Color m_cellColorB;
	sf::Color m_selectCellColor;
	sf::Color m_endCellColor;
	sf::Color m_pathCellColor;
	sf::Color m_buttonColor = { 141, 141, 141 };
	sf::Color m_buttonTextColor = { 0, 0, 0 };

	bool m_isWhitePlayerBot     = false;
	bool m_isBlackPlayerBot     = true;
	bool m_canSelectCell        = true;
	bool m_canBotSearch         = true;
	bool m_canBotMakeMove       = false;
	bool m_isGameOver           = false;
	bool m_isSearchParallel     = true;
#pragma endregion



#pragma region Initialization
	void loadFromConfig();
	void init();
	void resizeParam();
	void initBackground();
	void initTimer();
	void initBoard();
	void initMainButtons();
	void initGameOverPrompt();
	void initGameOverShape(Vec2 backShapeSize, Vec2 backShapePos);
	void initGameOverPromptText(Vec2 backShapeSize, Vec2 backShapePos);
	void initGameOverPromptButtons(Vec2 backShapeSize, Vec2 backShapePos);
	void resizeGame();
	void adjustCheckersCTransform(float relDiff, float prevOffsetX, float newOffsetX);
	void createCell(int row, int col, Vec2 pos, bool isCellWhite, bool isOccupied, bool isCheckerWhite);
	void createChecker(int row, int col, Vec2 pos, bool isCheckerWhite);
	void createChecker(int row, int col, bool isCheckerWhite, bool isKing);
	void addCheckerAnimation(Entity* checker, bool isCheckerWhite) const;
	void addCheckerKingAnimation(Entity* checker, bool isCheckerWhite) const;
#pragma endregion

#pragma region CanMoveAndAllPathsSearch
	void handleCanMoveAndAllPathsSearch(GameState& gameState) const;
	std::vector<Entity*> canMoveSearch(const GameState& gameState) const;
	bool isInCanMove(const std::vector<Entity*>& canMoveCells, int id) const;
	AllPathsSearchResult allPathsSearch(const GameState& gameState) const;
	bool isLegalActionLight(const std::array<std::array<CStateCell, 8>, 8>& allCellsState, sf::Vector2i nodeGPos, sf::Vector2i action) const;
	bool isOutOfBounds(sf::Vector2i gPos) const;
	bool isValidDirection(bool isCheckerWhite, sf::Vector2i action) const;
	bool pathSearch(std::array<std::array<CStateCell, 8>, 8>& allCellsState, std::unordered_map<int, std::vector<Path>>& pathsMap, std::vector<int>& endCells, sf::Vector2i nodeGPos, sf::Vector2i prevAction) const;
	std::pair<bool, bool> isLegalAction(const std::array<std::array<CStateCell, 8>, 8>& allCellsState, sf::Vector2i nodeGPos, sf::Vector2i action, bool isActionRepeating) const;
	void updateStatesOnJump(CStateCell& nodeS, CStateCell& moveNodeS, CStateCell& jumpNodeS) const;
	void updateStatesOnMove(CStateCell& nodeS, CStateCell& moveNodeS, bool isActionRepeating) const;
	bool isInEnd(const std::vector<int>& endCellsVec, int id) const;
	void trimPaths(std::unordered_map<int, std::vector<Path>>& pathsMap, std::vector<int>& endCells, bool jumpInPath) const;
#pragma endregion

#pragma region Update
	void update() override;
	void updateTimer();
	void onEnd() override;
	void pause();
#pragma endregion

#pragma region BotMove
	void botMove();
	bool isBotsTurn() const;
	void startSearch(std::stop_token stopToken, GameState gameState);
	std::vector<ThreadTask> generateThreadTasks(GameState& gameState) const;
	std::vector<EvaluatedMove> handleThreads(std::stop_token stopToken, std::vector<ThreadTask>& threadTasks) const;
	void threadFunc(std::stop_token stopToken, std::vector<ThreadTask>& threadTasks, int& nextTaskIndex, std::vector<EvaluatedMove>& moves, int& alpha, std::mutex& mtx) const;
	int alphaBetaSearch(std::stop_token stopToken, GameState& gameState, int alpha, int beta, int depth, bool maxTurn) const;
	int evalState(const GameState& gameState, bool maxTurn) const;
	void simulateMovement(GameState& gameState) const;
	void stopAlphaBetaSearch();
	EvaluatedMove getBestMove(const std::vector<EvaluatedMove>& moves) const;
#pragma endregion

#pragma region SaveAndSetGameState
	void saveGameState();
	void setGameState();
	void updateGameState();
	void resetCheckers();
	void resetCheckerAnimation(Entity* checker, int row, int col);
	void resetCheckerPos(Entity* checker, int row, int col);
	void stopSelectedChecker();
#pragma endregion

#pragma region DoActionSystem
	void sDoAction(const Action& action) override;
	void handleCellSelection();
	Entity* getClickedCell();
	bool canSelectPath(int cellID) const;
	void handlePathSelection(GameState& gameState, int endCellID) const;
	bool canSelectNewCell(int cellID) const;
	void handleSelectingNewCell(Entity* cell);
#pragma endregion

#pragma region MovementSystem
	void sMovement();
	bool hasReachedTarget(const CTransform& checkerT) const;
	sf::Vector2i getGPos(Vec2 pos) const;
	void handleEndOfCheckerPath(CTransform& checkerT);
	bool isGameOver();
	void showGameOverPrompt();
	Path getMaxJumpsPath(std::vector<Path>&& paths) const;
	void checkerCrowning(GameState& gameState, int targetCellRow, bool isSimulation) const;
	bool shouldCrowChecker(CStateCell selectedCellS, int targetCellRow) const;
	void updataCheckerAmount(GameState& gameState, const Entity* targetCell) const;
	void changeSelectedCell(GameState& gameState, Entity* targetCell) const;
	void changeChecker(const Entity* selectedCell, sf::Vector2i targetCellGPos);
	void changeCheckerTarget(int moveIndex) const;
#pragma endregion

#pragma region RenderSystem
	void sRender() override;
	void colorCells();
	sf::Color getSuitableCellColor(const Entity* cell) const;
	bool shouldColorPath(const Entity* cell) const;
	bool isInPath(int id) const;
#pragma endregion
};
