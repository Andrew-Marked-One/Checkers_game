#include "Scene_Play.h"
#include "Scene_Menu.h"


#pragma region Initialization
Scene_Play::Scene_Play(GameEngine* gameEngine, bool isWhitePlayerBot, bool isBlackPlayerBot)
	: Scene{ gameEngine }
	, m_isWhitePlayerBot{ isWhitePlayerBot }
	, m_isBlackPlayerBot{ isBlackPlayerBot } {

	loadFromConfig();
	init();
}

Scene_Play::~Scene_Play() {
	stopAlphaBetaSearch();
}

void Scene_Play::loadFromConfig() {
	std::ifstream file(m_game->getConfigPath());
	if (!file.is_open()) {
		std::cerr << "Scene_Play::loadFromConfig: cannot open file with path \"" << m_game->getConfigPath() << "\"\n";
	}
	std::string line;

	while (std::getline(file, line)) {
		std::stringstream iss(line);
		std::string type;
		int r, g, b;
		iss >> type;
		if (type == "SearchDepthLimit") {
			iss >> m_searchDepthLimitWhite >> m_searchDepthLimitBlack;
		}
		else if (type == "MovementSpeed") {
			iss >> m_movementSpeed;
		}
		else if (type == "RecordLimit") {
			int bufferSize;
			iss >> bufferSize;
			m_gameHistory.resize(bufferSize);
		}
		else if (type == "TimeLimit") {
			int timeLimit;
			iss >> timeLimit;
			m_searchTimer.setTimeLimit(timeLimit);
		}
		else if (type == "CellColorW") {
			iss >> r >> g >> b;
			m_cellColorW = sf::Color(r, g, b);
		}
		else if (type == "CellColorB") {
			iss >> r >> g >> b;
			m_cellColorB = sf::Color(r, g, b);
		}
		else if (type == "SelectCellColor") {
			iss >> r >> g >> b;
			m_selectCellColor = sf::Color(r, g, b);
		}
		else if (type == "EndCellColor") {
			iss >> r >> g >> b;
			m_endCellColor = sf::Color(r, g, b);
		}
		else if (type == "PathCellColor") {
			iss >> r >> g >> b;
			m_pathCellColor = sf::Color(r, g, b);
		}
	}
}

void Scene_Play::init() {
	resizeParam();

	registerAction(sf::Keyboard::Escape, "QUIT");

	initBackground();
	initTimer();
	initBoard();
	initMainButtons();
	initGameOverPrompt();

	m_entityManager.update();
	
	hideEntities("gameOverPrompt");

	resizeText("button", m_gridSize.x / 3.333f);

	saveGameState();

	handleCanMoveAndAllPathsSearch(m_gameState);

	m_mainTimer.start();
}

void Scene_Play::resizeParam() {
	resetWindowSize();

	float gridSize = std::min(m_windowSize.x / 10, m_windowSize.y / 8);
	m_gridSize = { gridSize, gridSize };
	m_gridHalfSize = m_gridSize / 2;

	float leftBound = m_windowHalfSize.x - m_gridSize.x * 4;
	float rightBound = leftBound + m_gridSize.x * 8;
	float bottomBound = m_gridSize.y * 8;
	m_gridBounds = { leftBound, 0.f, rightBound, bottomBound };

	m_movementSpeedScaled = m_movementSpeed * m_gridSize.x / m_game->getFramerateLimit();
}

void Scene_Play::initBackground() {
	Entity* background = m_entityManager.addEntity("background");
	background->addComponent<CTransform>(m_windowHalfSize);
	auto& animation = m_game->assets().getAnimation("background");
	auto& backgroundA = background->addComponent<CAnimation>(animation, true).animation;
	backgroundA.setScale(m_windowSize, false);
}

void Scene_Play::initTimer() {
	Vec2 textContainer = m_gridSize * 0.95;
	Vec2 textPos = { m_gridBounds.left - m_gridHalfSize.x, m_gridHalfSize.y };
	Entity* timer = createText("timer", "00:00", textContainer, textPos, "Marhey", sf::Color(141, 141, 141));
	auto& timerText = timer->getComponent<CText>().text;
	timerText.setOutlineColor(sf::Color(0, 0, 0));
	timerText.setOutlineThickness(-2.f);
}

void Scene_Play::initBoard() {
	bool isCellWhite = true;
	bool isCheckerWhite = false;
	bool isCheckerCreated = false;

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Vec2 pos = { col * m_gridSize.x + m_gridHalfSize.x + m_gridBounds.left, row * m_gridSize.y + m_gridHalfSize.y };
			if (!isCellWhite) {
				if (row < 3) {
					isCheckerWhite = false;
					isCheckerCreated = true;
					createChecker(row, col, pos, isCheckerWhite);
				}
				else if (row > 4) {
					isCheckerWhite = true;
					isCheckerCreated = true;
					createChecker(row, col, pos, isCheckerWhite);
				}
			}
			createCell(row, col, pos, isCellWhite, isCheckerCreated, isCheckerWhite);
			isCellWhite = !isCellWhite;
			isCheckerCreated = false;
		}
		isCellWhite = !isCellWhite;
	}
}

void Scene_Play::initMainButtons() {
	Vec2 buttonSize = { m_gridSize.x, m_gridHalfSize.y };
	Vec2 buttonPos	= { m_gridBounds.width + m_gridHalfSize.x, m_gridHalfSize.y / 2 };
	std::vector<std::string> tags = { "button", "mainButton" };

	Entity* buttonMenu = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Menu", "Marhey", m_buttonTextColor);
	buttonMenu->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_hasEnded = true;
	});

	buttonPos.y += buttonSize.y;
	Entity* buttonPause = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Pause", "Marhey", m_buttonTextColor);
	buttonPause->getComponent<CFunctions>().addFunc("leftClick", [this, button = buttonPause]() {
		auto& buttonSh = button->getComponent<CShape>().shape;
		static sf::Color baseColor = buttonSh.getFillColor();
		buttonSh.setFillColor(m_isPaused ? baseColor : sf::Color(190, 60, 60));
		pause();
	});

	buttonPos.y += buttonSize.y;
	Entity* buttonBack = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Back", "Marhey", m_buttonTextColor);
	buttonBack->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_gameHistory.back();
		setGameState();

		if (m_isGameOver) {
			m_isGameOver = false;
			hideEntities("gameOverPrompt");
		}
	});

	buttonPos.y += buttonSize.y;
	Entity* buttonForth = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Forth", "Marhey", m_buttonTextColor);
	buttonForth->getComponent<CFunctions>().addFunc("leftClick", [this]() {
		m_gameHistory.forth();
		setGameState();

		if (isGameOver()) {
			m_isGameOver = true;
			showGameOverPrompt();
		}
	});
}

void Scene_Play::resizeGame() {
	float prevGridSize = m_gridSize.x;
	float prevOffsetX = m_gridBounds.left;
	resizeParam();
	float relDiff = m_gridSize.x / prevGridSize;
	float newOffsetX = m_gridBounds.left;

	resizeEntities(relDiff, prevOffsetX, newOffsetX);
	resetBackgound();
	adjustCheckersCTransform(relDiff, prevOffsetX, newOffsetX);
}

void Scene_Play::adjustCheckersCTransform(float relDiff, float prevOffsetX, float newOffsetX) {
	for (Entity* checker : m_entityManager.getEntities("checker")) {
		auto& checkerT = checker->getComponent<CTransform>();
		checkerT.velocity *= relDiff;
		checkerT.target = { (checkerT.target.x - prevOffsetX) * relDiff + newOffsetX, checkerT.target.y * relDiff };
	}
}

void Scene_Play::initGameOverPrompt() {
	Vec2 backShapeSize = { m_gridSize.x * 3.5, m_gridSize.y * 2.5 };
	Vec2 backShapePos = { m_gridBounds.left + m_gridSize.x * 4, m_gridSize.y * 4 };

	initGameOverShape(backShapeSize, backShapePos);
	initGameOverPromptText(backShapeSize, backShapePos);
	initGameOverPromptButtons(backShapeSize, backShapePos);
}

void Scene_Play::initGameOverShape(Vec2 backShapeSize, Vec2 backShapePos) {
	Entity* backShape = m_entityManager.addEntity("gameOverPrompt");
	backShape->addComponent<CShape>(backShapeSize, sf::Color(50, 50, 50), sf::Color(0, 0, 0), -1);
	backShape->addComponent<CTransform>(backShapePos).pos;
}

void Scene_Play::initGameOverPromptText(Vec2 backShapeSize, Vec2 backShapePos) {
	Vec2 container = { backShapeSize.x * 0.9, m_gridSize.y };
	backShapePos.y -= m_gridHalfSize.y;

	Entity* textEntity = createText(std::vector<std::string>{ "gameOverPrompt", "gameOverPromptText" }, "No one wins", container, backShapePos, "Marhey", sf::Color(141, 141, 141));
	auto& textEntityText = textEntity->getComponent<CText>().text;
	textEntityText.setOutlineColor(sf::Color(0, 0, 0));
	textEntityText.setOutlineThickness(-2.f);
}

void Scene_Play::initGameOverPromptButtons(Vec2 backShapeSize, Vec2 backShapePos) {
	Vec2 buttonSize = { m_gridSize.x + m_gridHalfSize.x, m_gridHalfSize.y };
	Vec2 buttonPos = { backShapePos.x, backShapePos.y + m_gridHalfSize.y };
	std::vector<std::string> tags = { "button", "gameOverPrompt", "gameOverPromptButton" };

	if (m_isBlackPlayerBot && !m_isWhitePlayerBot || !m_isBlackPlayerBot && m_isWhitePlayerBot) {
		buttonPos.x -= buttonSize.x * 0.6;
		Entity* buttoChangeSides = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Flip sides", "Marhey", m_buttonTextColor);
		buttoChangeSides->getComponent<CFunctions>().addFunc("leftClick", [this]() {
			m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, !m_isWhitePlayerBot, !m_isBlackPlayerBot), true);
		});
		
		buttonPos.x += buttonSize.x * 1.2;
		Entity* buttoRetry = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Retry", "Marhey", m_buttonTextColor);
		buttoRetry->getComponent<CFunctions>().addFunc("leftClick", [this]() {
			m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, m_isWhitePlayerBot, m_isBlackPlayerBot), true);
		});
	}
	else {
		Entity* buttoRetry = createButton(tags, buttonSize, buttonPos, m_buttonColor, "Retry", "Marhey", m_buttonTextColor);
		buttoRetry->getComponent<CFunctions>().addFunc("leftClick", [this]() {
			m_game->changeScene("PLAY", std::make_unique<Scene_Play>(m_game, m_isWhitePlayerBot, m_isBlackPlayerBot), true);
		});
	}
}

void Scene_Play::createCell(int row, int col, Vec2 pos, bool isCellWhite, bool isOccupied, bool isCheckerWhite) {
	std::vector<std::string> cellTags = { "cell", isCellWhite ? "whiteCell" : "blackCell" };
	Entity* cell = m_entityManager.addEntity(cellTags);
	cell->addComponent<CTransform>(pos);
	cell->addComponent<CShape>(m_gridSize, isCellWhite ? m_cellColorW : m_cellColorB, sf::Color(0, 0, 0), -1);

	cell->row = row;
	cell->col = col;
	m_cells[row][col] = cell;

	auto& cellS = m_gameState.allCellsState[row][col];
	cellS.isWhite = isCellWhite;
	cellS.isOccupied = isOccupied;
	cellS.isOccupiedByWhite = isCheckerWhite;
}

void Scene_Play::createChecker(int row, int col, Vec2 pos, bool isCheckerWhite) {
	std::vector<std::string> checkerTags = { "checker", isCheckerWhite ? "whiteChecker" : "blackChecker" };
	Entity* checker = m_entityManager.addEntity(checkerTags);
	checker->addComponent<CTransform>(pos);
	addCheckerAnimation(checker, isCheckerWhite);

	checker->row = row;
	checker->col = col;
	m_cellToChecker[row][col] = checker;
}

void Scene_Play::createChecker(int row, int col, bool isCheckerWhite, bool isKing) {
	float posX = col * m_gridSize.x + m_gridBounds.left + m_gridHalfSize.x;
	float posY = row * m_gridSize.y + m_gridHalfSize.y;
	std::vector<std::string> checkerTags = { "checker", isCheckerWhite ? "whiteChecker" : "blackChecker" };
	Entity* checker = m_entityManager.addEntity(checkerTags);
	checker->addComponent<CTransform>(Vec2(posX, posY));
	isKing ? addCheckerKingAnimation(checker, isCheckerWhite) : addCheckerAnimation(checker, isCheckerWhite);

	checker->row = row;
	checker->col = col;
	m_cellToChecker[row][col] = checker;

	auto& cellS = m_gameState.allCellsState[row][col];
	cellS.isOccupied = true;
	cellS.isOccupiedByWhite = isCheckerWhite;
	cellS.isKing = isKing;
}


void Scene_Play::addCheckerAnimation(Entity* checker, bool isCheckerWhite) const {
	auto& animation = m_game->assets().getAnimation(isCheckerWhite ? "WhitePiece" : "BlackPiece");
	auto& checkerA = checker->addComponent<CAnimation>(animation, true).animation;
	checkerA.setScale(m_gridSize * 0.9, true);
}

void Scene_Play::addCheckerKingAnimation(Entity* checker, bool isCheckerWhite) const {
	auto& animation = m_game->assets().getAnimation(isCheckerWhite ? "WhitePieceKing" : "BlackPieceKing");
	auto& checkerA = checker->addComponent<CAnimation>(animation, true).animation;
	checkerA.setScale(m_gridSize * 0.9, true);
}
#pragma endregion



#pragma region CanMoveAndAllPathsSearch
void Scene_Play::handleCanMoveAndAllPathsSearch(GameState& gameState) const {
	gameState.canMoveCells = canMoveSearch(gameState);
	auto result = allPathsSearch(gameState);
	gameState.allEndCells = std::move(result.allEndCells);
	gameState.allPaths = std::move(result.allPaths);
	gameState.canMoveCells = std::move(result.canMoveCells);
}

std::vector<Entity*> Scene_Play::canMoveSearch(const GameState& gameState) const {
	std::vector<Entity*> canMoveCells;
	canMoveCells.reserve(12);

	auto& allCellsState = gameState.allCellsState;

	for (Entity* cell : m_entityManager.getEntities("blackCell")) {
		CStateCell cellS = allCellsState[cell->row][cell->col];
		if (cellS.isOccupied && gameState.isWhitesTurn == cellS.isOccupiedByWhite) {
			for (sf::Vector2i action : m_actions) {
				if (isLegalActionLight(allCellsState, sf::Vector2i{ cell->col, cell->row }, action) && !isInCanMove(canMoveCells, cell->id())) {
					canMoveCells.emplace_back(cell);
				}
			}
		}
	}

	return canMoveCells;
}

bool Scene_Play::isInCanMove(const std::vector<Entity*>& canMoveCells, int id) const {
	return std::ranges::find_if(canMoveCells, [id](const Entity* e) {
		return e->id() == id;
	}) != canMoveCells.end();
}

AllPathsSearchResult Scene_Play::allPathsSearch(const GameState& gameState) const {
	size_t canMoveCellsSize = gameState.canMoveCells.size();

	std::unordered_map<int, std::unordered_map<int, std::vector<Path>>> allPaths;
	allPaths.reserve(canMoveCellsSize);

	std::unordered_map<int, std::vector<int>> allEndCells;
	allEndCells.reserve(canMoveCellsSize);

	std::vector<Entity*> jumpCells;
	jumpCells.reserve(canMoveCellsSize);

	std::array<std::array<CStateCell, 8>, 8> allCellsState = gameState.allCellsState;

	for (Entity* cell : gameState.canMoveCells) {
		int cellID = cell->id();

		auto& pathsMap = allPaths[cellID];
		auto& endCells = allEndCells[cellID];

		pathsMap[cellID].emplace_back().idVec.emplace_back(cellID);

		bool jumpInPath = pathSearch(allCellsState, pathsMap, endCells, sf::Vector2i{ cell->col, cell->row }, sf::Vector2i{});
		trimPaths(pathsMap, endCells, jumpInPath);

		if (jumpInPath) {
			jumpCells.emplace_back(cell);
		}
	}

	return { std::move(allEndCells), std::move(allPaths),  jumpCells.empty() ? gameState.canMoveCells : std::move(jumpCells)};
}

bool Scene_Play::isLegalActionLight(const std::array<std::array<CStateCell, 8>, 8>& allCellsState, sf::Vector2i nodeGPos, sf::Vector2i action) const {
	sf::Vector2i moveGPos = nodeGPos + action;

	if (isOutOfBounds(moveGPos)) {
		return false;
	}

	CStateCell nodeS = allCellsState[nodeGPos.y][nodeGPos.x];
	CStateCell moveNodeS = allCellsState[moveGPos.y][moveGPos.x];

	if (moveNodeS.isOccupied) {
		sf::Vector2i jumpGPos = moveGPos + action;
		bool isTargetEnemy = nodeS.isOccupiedByWhite != moveNodeS.isOccupiedByWhite;

		if (isTargetEnemy && !isOutOfBounds(jumpGPos)) {
			CStateCell jumpNodeS = allCellsState[jumpGPos.y][jumpGPos.x];
			return !jumpNodeS.isOccupied;
		}
	}
	else if (nodeS.isKing || isValidDirection(nodeS.isOccupiedByWhite, action)) {
		return true;
	}
	return false;
}

bool Scene_Play::isOutOfBounds(sf::Vector2i gPos) const {
	return gPos.x < 0 || gPos.x > 7 || gPos.y < 0 || gPos.y > 7;
}

bool Scene_Play::isValidDirection(bool isCheckerWhite, sf::Vector2i action) const {
	if (isCheckerWhite) {
		return action == m_actions[0] || action == m_actions[1];
	}
	else {
		return action == m_actions[2] || action == m_actions[3];
	}
}

bool Scene_Play::pathSearch(std::array<std::array<CStateCell, 8>, 8>& allCellsState, std::unordered_map<int, std::vector<Path>>& pathsMap, std::vector<int>& endCells, sf::Vector2i nodeGPos, sf::Vector2i prevAction) const {
	bool hasNodeJumped = false;
	std::array<CStateCell, 3> tempStates;

	int nodeID = m_cells[nodeGPos.y][nodeGPos.x]->id();
	auto& nodeS = allCellsState[nodeGPos.y][nodeGPos.x];
	tempStates[0] = nodeS;
	int nodePathIndex = pathsMap[nodeID].size() - 1;

	for (int i = 0; i < 4; i++) {
		sf::Vector2i action = m_actions[i];

		if (action == -prevAction) {
			continue;
		}

		bool isActionRepeating = action == prevAction;
		auto [isLegal, needJumn] = isLegalAction(allCellsState, nodeGPos, action, isActionRepeating);

		if (isLegal) {
			sf::Vector2i moveGPos = nodeGPos + action;
			int moveNodeID = m_cells[moveGPos.y][moveGPos.x]->id();
			auto& moveNodeS = allCellsState[moveGPos.y][moveGPos.x];
			tempStates[1] = moveNodeS;

			Path& nodePath = pathsMap[nodeID][nodePathIndex];

			if (needJumn) {
				hasNodeJumped = true;

				sf::Vector2i jumpGPos = moveGPos + action;
				auto& jumpNodeS = allCellsState[jumpGPos.y][jumpGPos.x];
				tempStates[2] = jumpNodeS;

				updateStatesOnJump(nodeS, moveNodeS, jumpNodeS);

				int jumpNodeID = m_cells[jumpGPos.y][jumpGPos.x]->id();

				auto& jumpNodePaths = pathsMap[jumpNodeID];
				auto& jumpNodePath = jumpNodePaths.emplace_back(nodePath);
				jumpNodePath.addActionAndId(i, moveNodeID);
				jumpNodePath.addActionAndId(i, jumpNodeID);
				jumpNodePath.amountOfJumps++;
				int jumpNodePathIndex = jumpNodePaths.size() - 1;


				bool hasNextNodeJumped = pathSearch(allCellsState, pathsMap, endCells, jumpGPos, action);

				if (hasNextNodeJumped) {
					jumpNodePaths.erase(jumpNodePaths.begin() + jumpNodePathIndex);
				}
				else if (!isInEnd(endCells, jumpNodeID)) {
					endCells.emplace_back(jumpNodeID);
				}

				jumpNodeS = tempStates[2];
			}
			else {
				updateStatesOnMove(nodeS, moveNodeS, isActionRepeating);

				auto& moveNodePath = pathsMap[moveNodeID].emplace_back(nodePath);
				moveNodePath.addActionAndId(i, moveNodeID);
				int moveNodePathIndex = pathsMap[moveNodeID].size() - 1;

				bool hasNextNodeJumped = pathSearch(allCellsState, pathsMap, endCells, moveGPos, action);

				if (hasNextNodeJumped) {
					hasNodeJumped = true;
				}

				if (hasNextNodeJumped || moveNodeS.hasToJump) {
					auto& paths = pathsMap[moveNodeID];
					paths.erase(paths.begin() + moveNodePathIndex);
				}
				else if (!isInEnd(endCells, moveNodeID)) {
					endCells.emplace_back(moveNodeID);
				}
			}

			nodeS = tempStates[0];
			moveNodeS = tempStates[1];
		}
	}

	return hasNodeJumped;
}

std::pair<bool, bool> Scene_Play::isLegalAction(const std::array<std::array<CStateCell, 8>, 8>& allCellsState, sf::Vector2i nodeGPos, sf::Vector2i action, bool isActionRepeating) const {
	sf::Vector2i moveGPos = nodeGPos + action;

	if (isOutOfBounds(moveGPos)) {
		return { false, false };
	}

	CStateCell nodeS = allCellsState[nodeGPos.y][nodeGPos.x];
	CStateCell moveNodeS = allCellsState[moveGPos.y][moveGPos.x];

	if (nodeS.isKing) {
		if (moveNodeS.isOccupied) {
			sf::Vector2i jumpGPos = moveGPos + action;
			bool isTargetEnemy = nodeS.isOccupiedByWhite != moveNodeS.isOccupiedByWhite;

			if (isTargetEnemy && (nodeS.canJump || isActionRepeating) && !isOutOfBounds(jumpGPos)) {
				bool jumpNodeOccupied = allCellsState[jumpGPos.y][jumpGPos.x].isOccupied;

				if (!jumpNodeOccupied) {
					return { true, true };
				}
			}
		}
		else if (nodeS.canMove || isActionRepeating) {
			return { true, false };
		}
	}
	else {
		if (moveNodeS.isOccupied) {
			sf::Vector2i jumpGPos = moveGPos + action;
			bool isTargetEnemy = nodeS.isOccupiedByWhite != moveNodeS.isOccupiedByWhite;

			if (isTargetEnemy && nodeS.canJump && !isOutOfBounds(jumpGPos)) {
				bool jumpNodeOccupied = allCellsState[jumpGPos.y][jumpGPos.x].isOccupied;

				if (!jumpNodeOccupied) {
					return { true, true };
				}
			}
		}
		else if (nodeS.canMove && isValidDirection(nodeS.isOccupiedByWhite, action)) {
			return { true, false };
		}
	}

	return { false, false };
}

void Scene_Play::updateStatesOnJump(CStateCell& nodeS, CStateCell& moveNodeS, CStateCell& jumpNodeS) const {
	nodeS.isOccupied = false;
	moveNodeS.isOccupied = false;
	jumpNodeS.isOccupied = true;
	jumpNodeS.isOccupiedByWhite = nodeS.isOccupiedByWhite;
	jumpNodeS.isKing = nodeS.isKing;

	if (nodeS.isKing) {
		jumpNodeS.canMove = true;
		jumpNodeS.canJump = true;
		jumpNodeS.hasJumped = true;
	}
	else {
		jumpNodeS.canMove = false;
	}
}

void Scene_Play::updateStatesOnMove(CStateCell& nodeS, CStateCell& moveNodeS, bool isActionRepeating) const {
	nodeS.isOccupied = false;
	moveNodeS.isOccupied = true;
	moveNodeS.isOccupiedByWhite = nodeS.isOccupiedByWhite;
	moveNodeS.isKing = nodeS.isKing;
	
	if (nodeS.isKing) {
		if (nodeS.hasJumped && isActionRepeating) {
			moveNodeS.hasJumped = true;
			moveNodeS.canMove = true;
			moveNodeS.canJump = true;
		}
		else {
			moveNodeS.hasToJump = nodeS.hasToJump || (nodeS.hasJumped && !isActionRepeating);
			moveNodeS.canMove = false;
			moveNodeS.canJump = false;
			moveNodeS.hasJumped = false;
		}
	}
	else {
		moveNodeS.canMove = false;
		moveNodeS.canJump = false;
	}
}

bool Scene_Play::isInEnd(const std::vector<int>& endCellsVec, int id) const {
	return std::ranges::find(endCellsVec, id) != endCellsVec.end();
}

void Scene_Play::trimPaths(std::unordered_map<int, std::vector<Path>>& pathsMap, std::vector<int>& endCells, bool jumpInPath) const {
	auto eraseEndCells = std::ranges::remove_if(endCells, [&pathsMap, jumpInPath](int endCell) {
		auto& paths = pathsMap[endCell];
		
		auto erasePaths = std::ranges::remove_if(paths, [jumpInPath](const Path& path) {
			return jumpInPath && path.amountOfJumps == 0;
		});

		paths.erase(erasePaths.begin(), erasePaths.end());

		return paths.empty();
	});

	endCells.erase(eraseEndCells.begin(), eraseEndCells.end());
}
#pragma endregion



#pragma region Update
void Scene_Play::update() {
	try {
		if (m_isPaused) {
			return;
		}

		m_entityManager.update();
		updateTimer();
		botMove();
		sMovement();
		m_currentFrame++;
	}
	catch (const std::exception& e) {
		std::cerr << "Scene_Play::update, exception caught: " << e.what() << '\n';
	}
}

void Scene_Play::updateTimer() {
	int totalTimeInSec = std::min(m_mainTimer.timeElapsed() / 1000, 3599);
	int secs = totalTimeInSec % 60;
	int mins = (totalTimeInSec / 60) % 60;

	std::string text = (mins < 10 ? "0" : "") + std::to_string(mins) + ':' +
					   (secs < 10 ? "0" : "") + std::to_string(secs);

	Entity* timer = m_entityManager.getEntities("timer")[0];
	timer->getComponent<CText>().text.setString(text);
}

void Scene_Play::onEnd() {
	m_game->changeScene("MENU", std::make_unique<Scene_Menu>(m_game), true);
}

void Scene_Play::pause() {
	if (!m_isPaused) {
		stopAlphaBetaSearch();
	}
	m_mainTimer.pause();
	m_isPaused = !m_isPaused;
}
#pragma endregion



#pragma region BotMove
void Scene_Play::botMove() {
	if (!isBotsTurn() || !m_canSelectCell) {
		return;
	}

	auto& canMoveCells = m_gameState.canMoveCells;

	if (m_canBotSearch) {
		m_canBotSearch = false;

		if (canMoveCells.empty()) {
			m_canSelectCell = false;
			return;
		}

		m_bestCanMoveIndex = 0;
		m_bestEndIndex = 0;

		auto& endCells = m_gameState.allEndCells[canMoveCells[0]->id()];
		auto& paths = m_gameState.allPaths[canMoveCells[0]->id()][endCells[0]];

		if (canMoveCells.size() == 1 && endCells.size() == 1 && paths.size() == 1) {
			std::cout << "alphaBetaSearch took 0s (1 move)\n";
		}
		else {
			m_canBotMakeMove = false;
			m_currentSearchDepthLimit = m_gameState.isWhitesTurn ? m_searchDepthLimitWhite : m_searchDepthLimitBlack;
			m_searchThread = std::jthread([this](std::stop_token stopToken) {
				startSearch(stopToken, m_gameState);
			});
		}
	}
	else if (m_canBotMakeMove) {
		m_canBotSearch = true;
		m_canSelectCell = false;

		auto& selectedCell = m_gameState.selectedCell;
		selectedCell = canMoveCells[m_bestCanMoveIndex];
		int endCellID = m_gameState.allEndCells[selectedCell->id()][m_bestEndIndex];

		handlePathSelection(m_gameState, endCellID);
	}
}

bool Scene_Play::isBotsTurn() const {
	return m_gameState.isWhitesTurn && m_isWhitePlayerBot || !m_gameState.isWhitesTurn && m_isBlackPlayerBot;
}

void Scene_Play::startSearch(std::stop_token stopToken, GameState gameState) {
	m_searchTimer.start();

	std::vector<ThreadTask> threadTasks = generateThreadTasks(gameState);
	std::vector<EvaluatedMove> moves = handleThreads(stopToken, threadTasks);
	EvaluatedMove bestMove = getBestMove(moves);

	m_bestCanMoveIndex = bestMove.canMoveIndex;
	m_bestEndIndex = bestMove.endIndex;

	std::cout << "alphaBetaSearch took " << m_searchTimer.timeElapsed() / 1000.f << "s\n";

	if (stopToken.stop_requested()) {
		m_canBotSearch = true;
	}
	else {
		m_canBotMakeMove = true;
	}
}

std::vector<ThreadTask> Scene_Play::generateThreadTasks(GameState& gameState) const {
	std::array<std::array<CStateCell, 8>, 8> allCellsState = gameState.allCellsState;
	CheckerAmount checkerAmount = gameState.checkerAmount;
	bool isWhitesTurn = gameState.isWhitesTurn;
	auto canMoveCells = std::move(gameState.canMoveCells);
	auto allEndCells = std::move(gameState.allEndCells);
	auto allPaths = std::move(gameState.allPaths);

	std::vector<ThreadTask> threadTasks;
	threadTasks.reserve(canMoveCells.size());
	int taskId = 0;
	for (size_t i = 0; i < canMoveCells.size(); i++) {
		Entity* canMoveCell = canMoveCells[i];
		auto& endCells = allEndCells[canMoveCell->id()];
		for (size_t j = 0; j < endCells.size(); j++) {
			gameState.selectedCell = canMoveCell;

			std::vector<Path>& paths = allPaths[canMoveCell->id()][endCells[j]];
			gameState.path = getMaxJumpsPath(std::move(paths));

			simulateMovement(gameState);

			threadTasks.emplace_back(gameState, i, j, taskId++);

			gameState.allCellsState = allCellsState;
			gameState.checkerAmount = checkerAmount;
			gameState.isWhitesTurn = isWhitesTurn;
		}
	}

	return threadTasks;
}

std::vector<EvaluatedMove> Scene_Play::handleThreads(std::stop_token stopToken, std::vector<ThreadTask>& threadTasks) const {
	int numberOfThreads = m_isSearchParallel ? std::ceil(std::jthread::hardware_concurrency() / 3.f) : 1;

	std::vector<std::jthread> threads;
	threads.reserve(numberOfThreads);

	std::vector<EvaluatedMove> moves(threadTasks.size());

	int nextTaskIndex = 0;
	int alpha = std::numeric_limits<int>::min();
	std::mutex mtx;

	for (int i = 0; i < numberOfThreads; i++) {
		threads.emplace_back(std::jthread([this, stopToken, &threadTasks, &nextTaskIndex, &moves, &alpha, &mtx]() {
			threadFunc(stopToken, threadTasks, nextTaskIndex, moves, alpha, mtx);
		}));
	}

	return moves;
}

void Scene_Play::threadFunc(std::stop_token stopToken, std::vector<ThreadTask>& threadTasks, int& nextTaskIndex, std::vector<EvaluatedMove>& moves, int& alpha, std::mutex& mtx) const {
	while (true) {
		ThreadTask task;
		{
			std::scoped_lock<std::mutex> lock(mtx);
			if (nextTaskIndex == threadTasks.size()) {
				break;
			}
			task = std::move(threadTasks[nextTaskIndex++]);
		}

		if (stopToken.stop_requested()) {
			break;
		}

		int value = alphaBetaSearch(stopToken, task.gameState, alpha, std::numeric_limits<int>::max(), 1, false);
		moves[task.taskId] = { value, task.canMoveIndex, task.endIndex };

		std::scoped_lock<std::mutex> lock(mtx);
		alpha = std::max(alpha, value);
	}
}

int Scene_Play::alphaBetaSearch(std::stop_token stopToken, GameState& gameState, int alpha, int beta, int depth, bool maxTurn) const {
	gameState.canMoveCells = canMoveSearch(gameState);

	if (gameState.canMoveCells.empty() || depth == m_currentSearchDepthLimit || m_searchTimer.timeRanOut() || stopToken.stop_requested()) {
		return evalState(gameState, maxTurn);
	}

	std::array<std::array<CStateCell, 8>, 8> allCellsState = gameState.allCellsState;
	CheckerAmount checkerAmount = gameState.checkerAmount;
	bool isWhitesTurn = gameState.isWhitesTurn;

	AllPathsSearchResult allPathsSearchResult = allPathsSearch(gameState);
	auto& canMoveCells = allPathsSearchResult.canMoveCells;
	auto& allEndCells = allPathsSearchResult.allEndCells;
	auto& allPaths = allPathsSearchResult.allPaths;

	for (size_t i = 0; i < canMoveCells.size(); i++) {
		Entity* canMoveCell = canMoveCells[i];
		auto& endCells = allEndCells[canMoveCell->id()];
		for (size_t j = 0; j < endCells.size(); j++) {
			gameState.selectedCell = canMoveCell;

			std::vector<Path>& paths = allPaths[canMoveCell->id()][endCells[j]];
			gameState.path = getMaxJumpsPath(std::move(paths));

			simulateMovement(gameState);

			int value = alphaBetaSearch(stopToken, gameState, alpha, beta, depth + 1, !maxTurn);

			if (maxTurn) {
				if (value > alpha) {
					alpha = value;
				}
			}
			else if (value < beta) {
				beta = value;
			}

			gameState.allCellsState = allCellsState;
			gameState.checkerAmount = checkerAmount;
			gameState.isWhitesTurn = isWhitesTurn;

			if (alpha >= beta || stopToken.stop_requested()) {
				i = canMoveCells.size();
				break;
			}
		}
	}

	return maxTurn ? alpha : beta;
}

int Scene_Play::evalState(const GameState& gameState, bool maxTurn) const {
	CheckerAmount checkerAmount = gameState.checkerAmount;
	int whiteEval = checkerAmount.whiteCheckersAll - checkerAmount.whiteCheckersKing + checkerAmount.whiteCheckersKing * 2;
	int blackEval = checkerAmount.blackCheckersAll - checkerAmount.blackCheckersKing + checkerAmount.blackCheckersKing * 2;

	if (maxTurn) {
		if (gameState.isWhitesTurn) {
			return whiteEval - blackEval;
		}
		else {
			return blackEval - whiteEval;
		}
	}
	else {
		if (gameState.isWhitesTurn) {
			return blackEval - whiteEval;
		}
		else {
			return whiteEval - blackEval;
		}
	}
}

void Scene_Play::simulateMovement(GameState& gameState) const {
	auto& actionIndices = gameState.path.actionIndices;
	sf::Vector2i checkerPos = { gameState.selectedCell->col, gameState.selectedCell->row };

	while (!actionIndices.empty()) {
		checkerPos += m_actions[actionIndices[0]];
		actionIndices.erase(actionIndices.begin());

		Entity* targetCell = m_cells[checkerPos.y][checkerPos.x];

		checkerCrowning(gameState, targetCell->row, true);
		updataCheckerAmount(gameState, targetCell);
		changeSelectedCell(gameState, targetCell);
	}

	gameState.isWhitesTurn = !gameState.isWhitesTurn;
}

void Scene_Play::stopAlphaBetaSearch() {
	if (m_searchThread.joinable()) {
		m_searchThread.request_stop();
		m_searchThread.join();
	}
}

EvaluatedMove Scene_Play::getBestMove(const std::vector<EvaluatedMove>& moves) const {
	return *std::ranges::max_element(moves, [](const EvaluatedMove& a, const EvaluatedMove& b) {
		return a.value < b.value;
	});
}
#pragma endregion



#pragma region MovementSystem
void Scene_Play::sMovement() {
	auto& actionIndices = m_gameState.path.actionIndices;

	if (actionIndices.empty()) {
		return;
	}
	
	Entity* selectedCell = m_gameState.selectedCell;
	auto& checkerT = m_cellToChecker[selectedCell->row][selectedCell->col]->getComponent<CTransform>();
	checkerT.pos += checkerT.velocity;

	if (hasReachedTarget(checkerT)) {
		checkerT.pos = checkerT.target;

		sf::Vector2i targetCellGPos = getGPos(checkerT.target);
		Entity* targetCell = m_cells[targetCellGPos.y][targetCellGPos.x];

		checkerCrowning(m_gameState, targetCell->row, false);
		updataCheckerAmount(m_gameState, targetCell);
		changeSelectedCell(m_gameState, targetCell);

		changeChecker(selectedCell, targetCellGPos);

		actionIndices.erase(actionIndices.begin());
		if (actionIndices.empty()) {
			handleEndOfCheckerPath(checkerT);
		}
		else {
			changeCheckerTarget(actionIndices[0]);
		}
	}
}

bool Scene_Play::hasReachedTarget(const CTransform& checkerT) const {
	if (checkerT.velocity.x < 0) {
		return checkerT.pos.x <= checkerT.target.x;
	}
	else {
		return checkerT.pos.x >= checkerT.target.x;
	}
}

sf::Vector2i Scene_Play::getGPos(Vec2 pos) const {
	int col = static_cast<int>((pos.x - m_gridBounds.left) / m_gridSize.x);
	int row = static_cast<int>(pos.y / m_gridSize.y);
	return { col, row };
}

void Scene_Play::handleEndOfCheckerPath(CTransform& checkerT) {
	checkerT.velocity = { 0, 0 };

	m_gameState.isWhitesTurn = !m_gameState.isWhitesTurn;
	m_canSelectCell = true;
	m_gameState.selectedCell = nullptr;
	saveGameState();

	handleCanMoveAndAllPathsSearch(m_gameState);

	if (isGameOver()) {
		m_isGameOver = true;
		showGameOverPrompt();
	}
}

bool Scene_Play::isGameOver() {
	return m_gameState.canMoveCells.empty() || m_gameState.checkerAmount.blackCheckersAll == 0 || m_gameState.checkerAmount.whiteCheckersAll == 0;
}

void Scene_Play::showGameOverPrompt() {
	bool hasWhiteWon = m_gameState.checkerAmount.whiteCheckersAll != 0;
	std::string text = hasWhiteWon ? "White wins" : "Black wins";

	auto& textEntityCText = m_entityManager.getEntities("gameOverPromptText")[0]->getComponent<CText>();
	textEntityCText.text.setString(text);
	textEntityCText.centerText();

	resizeText("gameOverPromptText");
	revealEntities("gameOverPrompt");
}

Path Scene_Play::getMaxJumpsPath(std::vector<Path>&& paths) const {
	return *std::ranges::max_element(paths, [](const Path& a, const Path& b) {
		return a.amountOfJumps < b.amountOfJumps;
	});
}

void Scene_Play::checkerCrowning(GameState& gameState, int targetCellRow, bool isSimulation) const {
	Entity* selectedCell = gameState.selectedCell;
	auto& selectedCellS = gameState.allCellsState[selectedCell->row][selectedCell->col];
	if (shouldCrowChecker(selectedCellS, targetCellRow)) {
		if (!isSimulation) {
			Entity* checker = m_cellToChecker[selectedCell->row][selectedCell->col];
			addCheckerKingAnimation(checker, selectedCellS.isOccupiedByWhite);
		}
		selectedCellS.isOccupiedByWhite ? gameState.checkerAmount.whiteCheckersKing++ : gameState.checkerAmount.blackCheckersKing++;
		selectedCellS.isKing = true;
	}
}

bool Scene_Play::shouldCrowChecker(CStateCell selectedCellS, int targetCellRow) const {
	if (!selectedCellS.isKing) {
		if (selectedCellS.isOccupiedByWhite) {
			return targetCellRow == 0;
		}
		else {
			return targetCellRow == 7;
		}
	}
	else {
		return false;
	}
}

void Scene_Play::updataCheckerAmount(GameState& gameState, const Entity* targetCell) const {
	CStateCell targetCellS = gameState.allCellsState[targetCell->row][targetCell->col];
	if (targetCellS.isOccupied) {
		auto& checkerAmount = gameState.checkerAmount;
		if (targetCellS.isOccupiedByWhite) {
			checkerAmount.whiteCheckersAll--;
			if (targetCellS.isKing) {
				checkerAmount.whiteCheckersKing--;
			}
		}
		else {
			checkerAmount.blackCheckersAll--;
			if (targetCellS.isKing) {
				checkerAmount.blackCheckersKing--;
			}
		}
	}
}

void Scene_Play::changeSelectedCell(GameState& gameState, Entity* targetCell) const {
	auto& selectedCell = gameState.selectedCell;
	auto& selectedCellS = gameState.allCellsState[selectedCell->row][selectedCell->col];
	auto& targetCellS = gameState.allCellsState[targetCell->row][targetCell->col];
	targetCellS = selectedCellS;
	selectedCellS.isOccupied = false;
	selectedCellS.isKing = false;
	selectedCell = targetCell;
}

void Scene_Play::changeChecker(const Entity* selectedCell, sf::Vector2i targetCellGPos) {
	auto& targetChecker = m_cellToChecker[targetCellGPos.y][targetCellGPos.x];
	if (targetChecker) {
		targetChecker->hide();
	}
	auto& selectedChecker = m_cellToChecker[selectedCell->row][selectedCell->col];
	targetChecker = selectedChecker;
	selectedChecker = nullptr;
}

void Scene_Play::changeCheckerTarget(int moveIndex) const {
	Vec2 action = Vec2{ m_actions[moveIndex] };
	Entity* checker = m_cellToChecker[m_gameState.selectedCell->row][m_gameState.selectedCell->col];
	auto& checkerT = checker->getComponent<CTransform>();
	checkerT.velocity = action * m_movementSpeedScaled;
	checkerT.target = checkerT.pos + action * m_gridSize;
}
#pragma endregion



#pragma region SaveAndSetGameState
void Scene_Play::saveGameState() {
	m_gameHistory.push_back(GameStateHist{ m_gameState.allCellsState, m_cellToChecker, m_gameState.checkerAmount, m_gameState.isWhitesTurn });
}

void Scene_Play::setGameState() {
	stopAlphaBetaSearch();
	updateGameState();
	resetCheckers();

	handleCanMoveAndAllPathsSearch(m_gameState);
}

void Scene_Play::updateGameState() {
	auto& gameState = m_gameHistory.head();
	m_gameState.allCellsState = gameState.allCellsState;
	m_gameState.checkerAmount = gameState.checkerAmount;
	m_gameState.isWhitesTurn = gameState.isWhitesTurn;
	m_gameState.path.clear();
	m_cellToChecker = gameState.cellToChecker;

	m_canSelectCell = true;
	m_canBotSearch = true;
}

void Scene_Play::resetCheckers() {
	for (Entity* checker : m_entityManager.getEntities("checker")) {
		checker->hide();
	}

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			Entity* checker = m_cellToChecker[row][col];
			if (checker) {
				resetCheckerAnimation(checker, row, col);
				resetCheckerPos(checker, row, col);
				checker->reveal();
			}
		}
	}
	stopSelectedChecker();
}

void Scene_Play::resetCheckerAnimation(Entity* checker, int row, int col) {
	CStateCell cellS = m_gameState.allCellsState[row][col];
	if (cellS.isKing) {
		addCheckerKingAnimation(checker, cellS.isOccupiedByWhite);
	}
	else {
		addCheckerAnimation(checker, cellS.isOccupiedByWhite);
	}
}

void Scene_Play::resetCheckerPos(Entity* checker, int row, int col) {
	Vec2 cellPos = m_cells[row][col]->getComponent<CTransform>().pos;
	checker->getComponent<CTransform>().pos = cellPos;
}

void Scene_Play::stopSelectedChecker() {
	if (m_gameState.selectedCell) {
		auto& checker = m_cellToChecker[m_gameState.selectedCell->row][m_gameState.selectedCell->col];
		if (checker) {
			checker->getComponent<CTransform>().velocity = { 0, 0 };
		}
	}
}
#pragma endregion



#pragma region DoActionSystem
void Scene_Play::sDoAction(const Action& action) {
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
			if (!m_canSelectCell || isBotsTurn() || m_isGameOver) {
				return;
			}
			handleCellSelection();
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

void Scene_Play::handleCellSelection() {
	auto cell = getClickedCell();
	if (!cell) {
		m_gameState.selectedCell = nullptr;
		return;
	}

	if (canSelectPath(cell->id())) {
		m_canSelectCell = false;
		handlePathSelection(m_gameState, cell->id());
	}
	else if (canSelectNewCell(cell->id())) {
		handleSelectingNewCell(cell);
	}
	else {
		m_gameState.selectedCell = nullptr;
	}
}

Entity* Scene_Play::getClickedCell() {
	for (Entity* entity : m_entityManager.getEntities()) {
		if (entity->hasTag("cell") && isMouseInside(entity)) {
			return entity;
		}
	}
	return nullptr;
}

bool Scene_Play::canSelectPath(int cellID) const {
	if (m_gameState.selectedCell) {
		return isInEnd(m_gameState.allEndCells.at(m_gameState.selectedCell->id()), cellID);
	}
	return false;
}

void Scene_Play::handlePathSelection(GameState& gameState, int endCellID) const{
	std::vector<Path>& paths = gameState.allPaths[gameState.selectedCell->id()][endCellID];
	gameState.path = getMaxJumpsPath(std::move(paths));
	changeCheckerTarget(gameState.path.actionIndices[0]);
}

bool Scene_Play::canSelectNewCell(int cellID) const {
	return isInCanMove(m_gameState.canMoveCells, cellID);
}

void Scene_Play::handleSelectingNewCell(Entity* cell) {
	m_gameState.selectedCell = cell;
	m_gameState.path.idVec.clear();

	for (int endCellID : m_gameState.allEndCells[cell->id()]) {
		for (auto& path : m_gameState.allPaths[cell->id()][endCellID]) {
			m_gameState.path.idVec.insert(m_gameState.path.idVec.end(), path.idVec.begin(), path.idVec.end());
		}
	}
}

#pragma endregion



#pragma region RenderSystem
void Scene_Play::sRender() {
	colorCells();

	m_game->window().clear();

	drawEntitiesExcept("checker");
	drawEntities(m_gameState.isWhitesTurn ? "blackChecker" : "whiteChecker");
	drawEntities(m_gameState.isWhitesTurn ? "whiteChecker" : "blackChecker");
	drawEntities("gameOverPrompt");

	m_game->window().display();
}

void Scene_Play::colorCells() {
	for (Entity* cell : m_entityManager.getEntities("blackCell")) {
		auto& cellSh = cell->getComponent<CShape>().shape;
		sf::Color suitableColor = getSuitableCellColor(cell);
		cellSh.setFillColor(suitableColor);
	}
}

sf::Color Scene_Play::getSuitableCellColor(const Entity* cell) const {
	/*
	auto& cellS = m_gameState.allCellsState[cell->row][cell->col];
	if (cellS.isOccupied) {
		return sf::Color(255, 0, 0);
	}
	else {
		return sf::Color(0, 0, 0);
	}*/

	if (isBotsTurn() || m_isGameOver) {
		return m_cellColorB;
	}

	if (!m_gameState.selectedCell) {
		if (isInCanMove(m_gameState.canMoveCells, cell->id())) {
			return m_selectCellColor;
		}
	}
	else if (shouldColorPath(cell)) {
		auto& endCells = m_gameState.allEndCells.at(m_gameState.selectedCell->id());

		if (isInEnd(endCells, cell->id())) {
			return m_endCellColor;
		}
		else if (isInPath(cell->id())) {
			return m_pathCellColor;
		}
	}
	return m_cellColorB;
}

bool Scene_Play::shouldColorPath(const Entity* cell) const {
	Entity* checker = m_cellToChecker[m_gameState.selectedCell->row][m_gameState.selectedCell->col];
	return checker->getComponent<CTransform>().velocity == Vec2{ 0, 0 };
}

bool Scene_Play::isInPath(int id) const {
	auto& idVec = m_gameState.path.idVec;
	return std::ranges::find(idVec, id) != idVec.end();
}
#pragma endregion
