#ifndef UI_H
#define UI_H

#include <string>

#include "maze_logic.h"

// ---------------------------------------------------------------
// Console user interface (FRONTEND part).
// All input / output of the game lives here.
// ---------------------------------------------------------------

enum class Direction {
    None,       // unknown key
    Up,
    Down,
    Left,
    Right,
    Help,
    Inventory,
    Quit
};

enum MenuOption {
    MENU_PLAY = 1,            // free play, the player chooses the size
    MENU_CAMPAIGN = 2,        // 5 levels + bonus levels
    MENU_PLAY_GRAPHICS = 3,
    MENU_HELP = 4,
    MENU_EXIT = 5
};

// --- Screens -----------------------------------------------------
void clearScreen();
void showMenu();
void showHelp();
void showMaze(const Maze& maze);
void showStatus(const Maze& maze, bool showLevel);
void showInventory(const Maze& maze);
void showMessage(const std::string& message);
void showMoveResult(MoveResult result);
void showLevelComplete(const Maze& maze, int bonus, bool showLevel);
void showBonusUnlocked();
void showCampaignComplete(const Maze& maze);
void waitForEnter();

// --- Input with validation ---------------------------------------
int readMenuChoice();
int readMazeSize();
bool askYesNo(const std::string& question);
Direction readDirection();

#endif
