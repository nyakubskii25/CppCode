#pragma once

#include <string>

#include "types.h"

enum MenuOption {
    MENU_CAMPAIGN = 1,
    MENU_FREE_PLAY = 2,
    MENU_SCORES = 3,
    MENU_HELP = 4,
    MENU_EXIT = 5
};

void showMenu();
void showHelp();
void showGame(const GameState& game);
void showInventory(const GameState& game);
void showFullMap(const GameState& game);
void showLevelComplete(const GameState& game, int bonus);
void showBonusUnlocked();
void showCampaignComplete(const GameState& game);
void showScores(const ScoreTable& table);
void showNotice(const std::string& text);
void waitForKey();
bool askYesNo(const std::string& question);
int readMenuChoice();
int readFreeSize();
std::string readPlayerName();
