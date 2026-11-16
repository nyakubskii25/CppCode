#include "maze.h"

#include "grid.h"

using namespace std;

namespace {

const int DIRECTION_COUNT = 4;
const Direction DIRECTIONS[DIRECTION_COUNT] = {
    Direction::Up, Direction::Down, Direction::Left, Direction::Right
};

}

Maze makeEmptyMaze()
{
    Maze maze;
    maze.size = 0;
    maze.cells = nullptr;
    maze.seen = nullptr;
    maze.trodden = nullptr;
    maze.player = { 0, 0 };
    maze.start = { 0, 0 };
    maze.exit = { 0, 0 };
    maze.fogRadius = NO_FOG;
    maze.moves = 0;
    maze.mapUsed = false;
    return maze;
}

void allocateMaze(Maze& maze, int size)
{
    maze.size = size;
    maze.cells = createGrid<char>(size, CELL_WALL);
    maze.seen = createGrid<bool>(size, false);
    maze.trodden = createGrid<bool>(size, false);
    maze.start = { 1, 1 };
    maze.exit = { size - 2, size - 2 };
    maze.player = maze.start;
    maze.moves = 0;
    maze.mapUsed = false;
}

void destroyMaze(Maze& maze)
{
    destroyGrid(maze.cells, maze.size);
    destroyGrid(maze.seen, maze.size);
    destroyGrid(maze.trodden, maze.size);
    maze.size = 0;
}

bool isInside(const Maze& maze, int row, int col)
{
    return row >= 0 && row < maze.size && col >= 0 && col < maze.size;
}

Position stepFrom(Position position, Direction direction)
{
    switch (direction) {
    case Direction::Up: return { position.row - 1, position.col };
    case Direction::Down: return { position.row + 1, position.col };
    case Direction::Left: return { position.row, position.col - 1 };
    case Direction::Right: return { position.row, position.col + 1 };
    default: return position;
    }
}

Direction directionBetween(Position from, Position to)
{
    if (to.row < from.row) {
        return Direction::Up;
    }
    if (to.row > from.row) {
        return Direction::Down;
    }
    if (to.col < from.col) {
        return Direction::Left;
    }
    if (to.col > from.col) {
        return Direction::Right;
    }
    return Direction::None;
}

int** buildDistanceMap(const Maze& maze, Position from, bool blockLocked)
{
    int** distance = createGrid<int>(maze.size, -1);
    Position* queue = new Position[maze.size * maze.size];
    int head = 0;
    int tail = 0;

    distance[from.row][from.col] = 0;
    queue[tail++] = from;

    while (head < tail) {
        Position current = queue[head++];
        for (int i = 0; i < DIRECTION_COUNT; i++) {
            Position next = stepFrom(current, DIRECTIONS[i]);
            if (!isInside(maze, next.row, next.col)) {
                continue;
            }
            char cell = maze.cells[next.row][next.col];
            if (cell == CELL_WALL || (blockLocked && cell == CELL_LOCKED)) {
                continue;
            }
            if (distance[next.row][next.col] >= 0) {
                continue;
            }
            distance[next.row][next.col] = distance[current.row][current.col] + 1;
            queue[tail++] = next;
        }
    }

    delete[] queue;
    return distance;
}

int countCells(const Maze& maze, char symbol)
{
    int total = 0;
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            if (maze.cells[row][col] == symbol) {
                total++;
            }
        }
    }
    return total;
}

const char* directionName(Direction direction)
{
    switch (direction) {
    case Direction::Up: return "North";
    case Direction::Down: return "South";
    case Direction::Left: return "West";
    case Direction::Right: return "East";
    default: return "nowhere";
    }
}
