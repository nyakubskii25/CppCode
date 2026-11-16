#include "game.h"

#include <string>

#include "artifacts.h"
#include "generator.h"
#include "grid.h"
#include "inventory.h"
#include "maze.h"
#include "messagelog.h"

using namespace std;

namespace {

const int DIRECTION_COUNT = 4;
const Direction DIRECTIONS[DIRECTION_COUNT] = {
    Direction::Up, Direction::Down, Direction::Left, Direction::Right
};

void collectArtifact(GameState& game)
{
    int id = pickRandomArtifact(game.inventory);
    game.score += ARTIFACT_POINTS;
    if (id < 0) {
        addMessage(game.log, "An old relic! It is worth " + to_string(ARTIFACT_POINTS) + " points.");
        return;
    }
    addItem(game.inventory, ItemType::Artifact, id, 1);
    const ArtifactInfo& info = game.catalog[id];
    addMessage(game.log, "Artifact found: " + info.name + ". " + info.description);
}

}

GameState* createGame(bool campaign)
{
    GameState* game = new GameState;
    game->maze = makeEmptyMaze();
    game->inventory = createInventory();
    game->catalog = createCatalog(game->catalogCount);
    game->log = createLog();
    game->level = 1;
    game->score = 0;
    game->totalMoves = 0;
    game->campaign = campaign;
    return game;
}

void destroyGame(GameState*& game)
{
    if (game == nullptr) {
        return;
    }
    destroyMaze(game->maze);
    destroyInventory(game->inventory);
    destroyCatalog(game->catalog);
    clearLog(game->log);
    delete game;
    game = nullptr;
}

void startLevel(GameState& game, int level, int freeSize)
{
    LevelConfig config = game.campaign ? getLevelConfig(level) : getFreePlayConfig(freeSize);
    generateMaze(game.maze, config);
    game.level = level;
    updateVision(game);

    if (game.campaign && config.bonus) {
        addMessage(game.log, "Bonus level " + to_string(level) + ". The maze is huge and dark!");
    } else if (game.campaign) {
        addMessage(game.log, "Level " + to_string(level) + " started. Find the exit E.");
    } else {
        addMessage(game.log, "Free play started. Find the exit E.");
    }
}

int currentVision(const GameState& game)
{
    if (game.maze.fogRadius == NO_FOG) {
        return NO_FOG;
    }
    return game.maze.fogRadius + visionBonus(game.inventory);
}

bool isVisible(const GameState& game, int row, int col)
{
    int radius = currentVision(game);
    if (radius == NO_FOG) {
        return true;
    }
    int rowGap = row - game.maze.player.row;
    int colGap = col - game.maze.player.col;
    return rowGap * rowGap + colGap * colGap <= radius * radius;
}

void updateVision(GameState& game)
{
    Maze& maze = game.maze;
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            if (isVisible(game, row, col)) {
                maze.seen[row][col] = true;
            }
        }
    }
}

MoveResult movePlayer(GameState& game, Direction direction)
{
    Maze& maze = game.maze;
    Position next = stepFrom(maze.player, direction);
    if (!isInside(maze, next.row, next.col)) {
        return MoveResult::Blocked;
    }

    char cell = maze.cells[next.row][next.col];
    MoveResult result = MoveResult::Moved;

    switch (cell) {
    case CELL_WALL:
        addMessage(game.log, "A solid wall. Press B to break it with a pickaxe.");
        return MoveResult::Blocked;
    case CELL_LOCKED:
        if (!removeItem(game.inventory, ItemType::Key, 1)) {
            addMessage(game.log, "The door is locked. You need a key.");
            return MoveResult::LockedDoor;
        }
        addMessage(game.log, "You opened a locked door with a key.");
        result = MoveResult::OpenedLockedDoor;
        break;
    case CELL_DOOR:
        addMessage(game.log, "You opened a door.");
        result = MoveResult::OpenedDoor;
        break;
    case CELL_COIN:
        addItem(game.inventory, ItemType::Coin, -1, 1);
        game.score += COIN_POINTS * coinMultiplier(game.inventory);
        addMessage(game.log, "You picked up a coin.");
        result = MoveResult::CollectedCoin;
        break;
    case CELL_KEY:
        addItem(game.inventory, ItemType::Key, -1, 1);
        addMessage(game.log, "You found a key.");
        result = MoveResult::CollectedKey;
        break;
    case CELL_PICKAXE:
        addItem(game.inventory, ItemType::Pickaxe, -1, 1);
        addMessage(game.log, "You found a pickaxe. Press B to break a wall.");
        result = MoveResult::CollectedPickaxe;
        break;
    case CELL_ARTIFACT:
        collectArtifact(game);
        result = MoveResult::CollectedArtifact;
        break;
    default:
        break;
    }

    if (cell != CELL_EXIT) {
        maze.cells[next.row][next.col] = CELL_PATH;
    }

    maze.trodden[maze.player.row][maze.player.col] = true;
    maze.player = next;
    maze.moves++;
    game.totalMoves++;
    updateVision(game);
    return result;
}

bool breakWall(GameState& game, Direction direction)
{
    Maze& maze = game.maze;
    if (countOf(game.inventory, ItemType::Pickaxe) == 0) {
        addMessage(game.log, "You have no pickaxe.");
        return false;
    }

    Position target = stepFrom(maze.player, direction);
    bool inner = target.row >= 1 && target.row <= maze.size - 2
              && target.col >= 1 && target.col <= maze.size - 2;
    if (!inner || maze.cells[target.row][target.col] != CELL_WALL) {
        addMessage(game.log, "There is no wall you can break there.");
        return false;
    }

    removeItem(game.inventory, ItemType::Pickaxe, 1);
    maze.cells[target.row][target.col] = CELL_PATH;
    addMessage(game.log, "You broke the wall to the " + string(directionName(direction)) + ".");
    updateVision(game);
    return true;
}

bool hasWon(const GameState& game)
{
    return game.maze.player.row == game.maze.exit.row
        && game.maze.player.col == game.maze.exit.col;
}

int finishLevel(GameState& game)
{
    int bonus = game.maze.size * LEVEL_BONUS_PER_CELL - game.maze.moves;
    if (bonus < MIN_LEVEL_BONUS) {
        bonus = MIN_LEVEL_BONUS;
    }
    bonus += countOf(game.inventory, ItemType::Key) * KEY_BONUS_POINTS;
    game.score += bonus;
    return bonus;
}

void useCompass(GameState& game)
{
    if (!hasArtifact(game.inventory, ARTIFACT_COMPASS)) {
        addMessage(game.log, "You need the Golden Compass for that.");
        return;
    }

    Maze& maze = game.maze;
    int** distance = buildDistanceMap(maze, maze.exit, false);
    int here = distance[maze.player.row][maze.player.col];
    Direction best = Direction::None;

    for (int i = 0; i < DIRECTION_COUNT; i++) {
        Position next = stepFrom(maze.player, DIRECTIONS[i]);
        if (isInside(maze, next.row, next.col) && distance[next.row][next.col] >= 0
            && distance[next.row][next.col] == here - 1) {
            best = DIRECTIONS[i];
            break;
        }
    }

    if (here < 0 || best == Direction::None) {
        addMessage(game.log, "The compass spins around. No way to the exit.");
    } else {
        addMessage(game.log, "The compass points " + string(directionName(best))
                   + ". The exit is " + to_string(here) + " steps away.");
    }
    destroyGrid(distance, maze.size);
}

bool useMap(GameState& game)
{
    if (!hasArtifact(game.inventory, ARTIFACT_MAP)) {
        addMessage(game.log, "You need the Ancient Map for that.");
        return false;
    }
    if (game.maze.mapUsed) {
        addMessage(game.log, "You already used the map on this level.");
        return false;
    }
    game.maze.mapUsed = true;
    return true;
}
