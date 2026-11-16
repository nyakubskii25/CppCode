#include "generator.h"

#include <cstdlib>

#include "grid.h"
#include "maze.h"

using namespace std;

namespace {

const int DIRECTION_COUNT = 4;
const Direction DIRECTIONS[DIRECTION_COUNT] = {
    Direction::Up, Direction::Down, Direction::Left, Direction::Right
};
const int ROW_STEPS[DIRECTION_COUNT] = { -1, 1, 0, 0 };
const int COL_STEPS[DIRECTION_COUNT] = { 0, 0, -1, 1 };
const int PLACEMENT_ATTEMPTS = 6000;
const int MIN_GATE_PATH = 8;

const LevelConfig LEVEL_TABLE[TOTAL_LEVELS] = {
    { 11, NO_FOG, false, 2, 0, 8, 0, 0, false },
    { 15, NO_FOG, true, 3, 0, 12, 1, 1, false },
    { 21, 8, true, 4, 1, 16, 1, 1, false },
    { 25, 7, true, 5, 2, 20, 2, 1, false },
    { 31, 7, true, 6, 3, 25, 2, 1, false },
    { 35, 6, true, 8, 4, 30, 3, 2, true },
    { 41, 6, true, 10, 5, 36, 3, 2, true },
    { 49, 5, true, 12, 6, 45, 4, 2, true }
};

int randomBelow(int limit)
{
    return rand() % limit;
}

void carvePassages(Maze& maze)
{
    Position* stack = new Position[maze.size * maze.size];
    int top = 0;

    maze.cells[1][1] = CELL_PATH;
    stack[top++] = { 1, 1 };

    while (top > 0) {
        Position current = stack[top - 1];
        int options[DIRECTION_COUNT];
        int optionCount = 0;

        for (int i = 0; i < DIRECTION_COUNT; i++) {
            int nextRow = current.row + ROW_STEPS[i] * 2;
            int nextCol = current.col + COL_STEPS[i] * 2;
            bool inside = nextRow >= 1 && nextRow <= maze.size - 2
                       && nextCol >= 1 && nextCol <= maze.size - 2;
            if (inside && maze.cells[nextRow][nextCol] == CELL_WALL) {
                options[optionCount++] = i;
            }
        }

        if (optionCount == 0) {
            top--;
            continue;
        }

        int chosen = options[randomBelow(optionCount)];
        maze.cells[current.row + ROW_STEPS[chosen]][current.col + COL_STEPS[chosen]] = CELL_PATH;
        Position next = { current.row + ROW_STEPS[chosen] * 2, current.col + COL_STEPS[chosen] * 2 };
        maze.cells[next.row][next.col] = CELL_PATH;
        stack[top++] = next;
    }

    delete[] stack;
}

int placeShortcuts(Maze& maze, char symbol, int count)
{
    int placed = 0;
    for (int attempt = 0; attempt < PLACEMENT_ATTEMPTS && placed < count; attempt++) {
        int row = 1 + randomBelow(maze.size - 2);
        int col = 1 + randomBelow(maze.size - 2);
        if (maze.cells[row][col] != CELL_WALL) {
            continue;
        }
        bool verticalGap = maze.cells[row - 1][col] == CELL_PATH
                        && maze.cells[row + 1][col] == CELL_PATH;
        bool horizontalGap = maze.cells[row][col - 1] == CELL_PATH
                          && maze.cells[row][col + 1] == CELL_PATH;
        if (verticalGap || horizontalGap) {
            maze.cells[row][col] = symbol;
            placed++;
        }
    }
    return placed;
}

bool placeGate(Maze& maze)
{
    int** distance = buildDistanceMap(maze, maze.exit, false);
    int length = distance[maze.start.row][maze.start.col];
    bool placed = false;

    if (length >= MIN_GATE_PATH) {
        Position* path = new Position[length + 1];
        int pathCount = 0;
        Position current = maze.start;
        path[pathCount++] = current;

        while (current.row != maze.exit.row || current.col != maze.exit.col) {
            Position chosen = current;
            for (int i = 0; i < DIRECTION_COUNT; i++) {
                Position next = stepFrom(current, DIRECTIONS[i]);
                if (isInside(maze, next.row, next.col)
                    && distance[next.row][next.col] == distance[current.row][current.col] - 1
                    && maze.cells[next.row][next.col] != CELL_WALL) {
                    chosen = next;
                    break;
                }
            }
            current = chosen;
            path[pathCount++] = current;
        }

        int low = length * 45 / 100;
        int high = length * 75 / 100;
        for (int attempt = 0; attempt < 100 && !placed; attempt++) {
            int index = low + randomBelow(high - low + 1);
            Position spot = path[index];
            if (maze.cells[spot.row][spot.col] == CELL_PATH) {
                maze.cells[spot.row][spot.col] = CELL_LOCKED;
                placed = true;
            }
        }
        delete[] path;
    }

    destroyGrid(distance, maze.size);
    return placed;
}

Position* collectFreeCells(const Maze& maze, int** reach, int& count)
{
    Position* pool = new Position[maze.size * maze.size];
    count = 0;
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            bool isStart = row == maze.start.row && col == maze.start.col;
            bool isFree = maze.cells[row][col] == CELL_PATH && !isStart;
            bool reachable = reach == nullptr || reach[row][col] >= 0;
            if (isFree && reachable) {
                pool[count++] = { row, col };
            }
        }
    }
    return pool;
}

bool placeOnRandomCell(Maze& maze, Position* pool, int& poolCount, char symbol)
{
    if (poolCount == 0) {
        return false;
    }
    int index = randomBelow(poolCount);
    Position spot = pool[index];
    pool[index] = pool[--poolCount];
    maze.cells[spot.row][spot.col] = symbol;
    return true;
}

void placeItems(Maze& maze, char symbol, int count, Position* pool, int& poolCount)
{
    for (int i = 0; i < count; i++) {
        placeOnRandomCell(maze, pool, poolCount, symbol);
    }
}

}

int normalizeSize(int requestedSize)
{
    int size = requestedSize;
    if (size < MIN_FREE_SIZE) {
        size = MIN_FREE_SIZE;
    }
    if (size > MAX_FREE_SIZE) {
        size = MAX_FREE_SIZE;
    }
    if (size % 2 == 0) {
        size++;
    }
    return size;
}

LevelConfig getLevelConfig(int level)
{
    int index = level - 1;
    if (index < 0) {
        index = 0;
    }
    if (index >= TOTAL_LEVELS) {
        index = TOTAL_LEVELS - 1;
    }
    return LEVEL_TABLE[index];
}

LevelConfig getFreePlayConfig(int requestedSize)
{
    int size = normalizeSize(requestedSize);
    LevelConfig config;
    config.size = size;
    config.fogRadius = NO_FOG;
    config.gateDoor = size >= 15;
    config.doors = size / 5;
    config.lockedDoors = size / 10;
    config.coins = size / 2;
    config.pickaxes = size / 15 + 1;
    config.artifacts = size / 20 + 1;
    config.bonus = false;
    return config;
}

void generateMaze(Maze& maze, const LevelConfig& config)
{
    destroyMaze(maze);
    allocateMaze(maze, config.size);
    maze.fogRadius = config.fogRadius;

    carvePassages(maze);
    maze.cells[maze.exit.row][maze.exit.col] = CELL_EXIT;

    placeShortcuts(maze, CELL_DOOR, config.doors);
    placeShortcuts(maze, CELL_LOCKED, config.lockedDoors);
    if (config.gateDoor) {
        placeGate(maze);
    }

    int lockedTotal = countCells(maze, CELL_LOCKED);
    if (lockedTotal > 0) {
        int** reach = buildDistanceMap(maze, maze.start, true);
        int reachCount = 0;
        Position* reachPool = collectFreeCells(maze, reach, reachCount);
        placeItems(maze, CELL_KEY, lockedTotal + 1, reachPool, reachCount);
        delete[] reachPool;
        destroyGrid(reach, maze.size);
    }

    int poolCount = 0;
    Position* pool = collectFreeCells(maze, nullptr, poolCount);
    placeItems(maze, CELL_COIN, config.coins, pool, poolCount);
    placeItems(maze, CELL_PICKAXE, config.pickaxes, pool, poolCount);
    placeItems(maze, CELL_ARTIFACT, config.artifacts, pool, poolCount);
    delete[] pool;
}
