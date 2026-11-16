#pragma once

#include "types.h"

GameState* createGame(bool campaign);
void destroyGame(GameState*& game);
void startLevel(GameState& game, int level, int freeSize);
int currentVision(const GameState& game);
bool isVisible(const GameState& game, int row, int col);
void updateVision(GameState& game);
MoveResult movePlayer(GameState& game, Direction direction);
bool breakWall(GameState& game, Direction direction);
bool hasWon(const GameState& game);
int finishLevel(GameState& game);
void useCompass(GameState& game);
bool useMap(GameState& game);
