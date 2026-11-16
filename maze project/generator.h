#pragma once

#include "types.h"

LevelConfig getLevelConfig(int level);
LevelConfig getFreePlayConfig(int requestedSize);
int normalizeSize(int requestedSize);
void generateMaze(Maze& maze, const LevelConfig& config);
