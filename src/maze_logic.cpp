// TEMPORARY backend implementation, so the frontend can be built and tested.
// The Backend Developer replaces this file with the real logic.

#include "maze_logic.h"

#include <cstdlib>

using namespace std;

namespace {

const int DIRECTION_COUNT = 4;
const int ROW_STEPS[DIRECTION_COUNT] = { -1, 1, 0, 0 };
const int COL_STEPS[DIRECTION_COUNT] = { 0, 0, -1, 1 };

// Maze size of every level: 5 campaign levels, then 3 bonus levels.
const int LEVEL_SIZES[TOTAL_LEVELS] = { 10, 15, 20, 25, 30, 40, 45, 50 };

const int LEVEL_BONUS_MIN = 50;
const int LEVEL_BONUS_PER_SIZE = 10;

// Item amounts depend on the maze size.
const int DOOR_DIVISOR = 5;
const int LOCKED_DOOR_DIVISOR = 6;
const int KEY_DIVISOR = 10;
const int PICKAXE_DIVISOR = 15;
const int BIG_MAZE_SIZE = 40;
const int PLACEMENT_ATTEMPTS = 2000;

// Recursive backtracker: carves passages two cells at a time.
void carvePassages(Maze& maze, int row, int col)
{
    maze.cells[row][col] = CELL_PATH;

    int order[DIRECTION_COUNT] = { 0, 1, 2, 3 };
    for (int i = DIRECTION_COUNT - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = order[i];
        order[i] = order[j];
        order[j] = tmp;
    }

    for (int i = 0; i < DIRECTION_COUNT; i++) {
        int rowStep = ROW_STEPS[order[i]];
        int colStep = COL_STEPS[order[i]];
        int nextRow = row + rowStep * 2;
        int nextCol = col + colStep * 2;

        bool inside = nextRow >= 0 && nextRow < maze.size
                   && nextCol >= 0 && nextCol < maze.size;
        if (inside && maze.cells[nextRow][nextCol] == CELL_WALL) {
            maze.cells[row + rowStep][col + colStep] = CELL_PATH;
            carvePassages(maze, nextRow, nextCol);
        }
    }
}

// Replaces walls that separate two passages with a door symbol.
// Such doors are only shortcuts, so they can never block the way.
void placeShortcuts(Maze& maze, char symbol, int count)
{
    for (int attempt = 0; attempt < PLACEMENT_ATTEMPTS && count > 0;
         attempt++) {
        int row = 1 + rand() % (maze.size - 2);
        int col = 1 + rand() % (maze.size - 2);
        if (maze.cells[row][col] != CELL_WALL) {
            continue;
        }

        bool verticalGap = maze.cells[row - 1][col] == CELL_PATH
                        && maze.cells[row + 1][col] == CELL_PATH;
        bool horizontalGap = maze.cells[row][col - 1] == CELL_PATH
                          && maze.cells[row][col + 1] == CELL_PATH;
        if (verticalGap || horizontalGap) {
            maze.cells[row][col] = symbol;
            count--;
        }
    }
}

// Puts items on random free passage cells (never on the start).
void scatterItems(Maze& maze, char symbol, int count)
{
    for (int attempt = 0; attempt < PLACEMENT_ATTEMPTS && count > 0;
         attempt++) {
        int row = rand() % maze.size;
        int col = rand() % maze.size;
        bool isStart = row == 0 && col == 0;
        if (!isStart && maze.cells[row][col] == CELL_PATH) {
            maze.cells[row][col] = symbol;
            count--;
        }
    }
}

void populateMaze(Maze& maze)
{
    int size = maze.size;
    placeShortcuts(maze, CELL_DOOR, size / DOOR_DIVISOR);
    placeShortcuts(maze, CELL_LOCKED, size / LOCKED_DOOR_DIVISOR);

    scatterItems(maze, CELL_COIN, size);
    scatterItems(maze, CELL_KEY, size / KEY_DIVISOR + 1);
    scatterItems(maze, CELL_PICKAXE, size / PICKAXE_DIVISOR + 1);
    scatterItems(maze, CELL_ARTIFACT, size >= BIG_MAZE_SIZE ? 2 : 1);
}

} // namespace

bool isValidMazeSize(int size)
{
    return size >= MIN_MAZE_SIZE && size <= MAX_MAZE_SIZE;
}

int getLevelSize(int level)
{
    if (level < 1 || level > TOTAL_LEVELS) {
        return 0;
    }
    return LEVEL_SIZES[level - 1];
}

bool isBonusLevel(int level)
{
    return level > CAMPAIGN_LEVELS;
}

bool createMaze(Maze& maze, int size)
{
    if (!isValidMazeSize(size)) {
        return false;
    }

    maze.size = size;
    maze.playerRow = 0;
    maze.playerCol = 0;
    maze.moves = 0;
    maze.level = 1;
    maze.score = 0;
    maze.inventory = { 0, 0, 0, 0 };

    maze.cells = new char*[size];
    for (int row = 0; row < size; row++) {
        maze.cells[row] = new char[size];
        for (int col = 0; col < size; col++) {
            maze.cells[row][col] = CELL_WALL;
        }
    }

    carvePassages(maze, 0, 0);

    // For even sizes the last column is not reached by the carver,
    // so connect the exit manually.
    if (size % 2 == 0) {
        maze.cells[size - 2][size - 1] = CELL_PATH;
    }
    maze.cells[size - 1][size - 1] = CELL_EXIT;

    populateMaze(maze);
    return true;
}

bool advanceLevel(Maze& maze, int newSize)
{
    if (!isValidMazeSize(newSize)) {
        return false;
    }

    Inventory savedInventory = maze.inventory;
    int savedScore = maze.score;
    int nextLevel = maze.level + 1;

    destroyMaze(maze);
    if (!createMaze(maze, newSize)) {
        return false;
    }

    maze.inventory = savedInventory;
    maze.score = savedScore;
    maze.level = nextLevel;
    return true;
}

void destroyMaze(Maze& maze)
{
    if (maze.cells != nullptr) {
        for (int row = 0; row < maze.size; row++) {
            delete[] maze.cells[row];
        }
        delete[] maze.cells;
        maze.cells = nullptr;
    }
    maze.size = 0;
}

MoveResult movePlayer(Maze& maze, int rowDelta, int colDelta)
{
    int newRow = maze.playerRow + rowDelta;
    int newCol = maze.playerCol + colDelta;

    bool inside = newRow >= 0 && newRow < maze.size
               && newCol >= 0 && newCol < maze.size;
    if (!inside) {
        return MoveResult::Blocked;
    }

    MoveResult result = MoveResult::Moved;
    char target = maze.cells[newRow][newCol];

    switch (target) {
    case CELL_WALL:
        if (maze.inventory.pickaxes <= 0) {
            return MoveResult::Blocked;
        }
        maze.inventory.pickaxes--;
        result = MoveResult::BrokeWall;
        break;
    case CELL_LOCKED:
        if (maze.inventory.keys <= 0) {
            return MoveResult::LockedDoor;
        }
        maze.inventory.keys--;
        result = MoveResult::OpenedLockedDoor;
        break;
    case CELL_COIN:
        maze.inventory.coins++;
        maze.score += COIN_POINTS;
        result = MoveResult::CollectedCoin;
        break;
    case CELL_KEY:
        maze.inventory.keys++;
        result = MoveResult::CollectedKey;
        break;
    case CELL_PICKAXE:
        maze.inventory.pickaxes++;
        result = MoveResult::CollectedPickaxe;
        break;
    case CELL_ARTIFACT:
        maze.inventory.artifacts++;
        maze.score += ARTIFACT_POINTS;
        result = MoveResult::CollectedArtifact;
        break;
    default:
        break;
    }

    // Broken walls, opened doors and collected items become plain path.
    if (result != MoveResult::Moved) {
        maze.cells[newRow][newCol] = CELL_PATH;
    }

    maze.playerRow = newRow;
    maze.playerCol = newCol;
    maze.moves++;
    return result;
}

bool hasPlayerWon(const Maze& maze)
{
    return maze.cells[maze.playerRow][maze.playerCol] == CELL_EXIT;
}

int finishLevel(Maze& maze)
{
    int bonus = maze.size * LEVEL_BONUS_PER_SIZE - maze.moves;
    if (bonus < LEVEL_BONUS_MIN) {
        bonus = LEVEL_BONUS_MIN;
    }
    maze.score += bonus;
    return bonus;
}
