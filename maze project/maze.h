#pragma once

#include "types.h"

void allocateMaze(Maze& maze, int size);
void destroyMaze(Maze& maze);
Maze makeEmptyMaze();
bool isInside(const Maze& maze, int row, int col);
Position stepFrom(Position position, Direction direction);
Direction directionBetween(Position from, Position to);
int** buildDistanceMap(const Maze& maze, Position from, bool blockLocked);
int countCells(const Maze& maze, char symbol);
const char* directionName(Direction direction);
