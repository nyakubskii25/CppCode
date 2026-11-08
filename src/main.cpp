#include <cstdlib>
#include <ctime>
#include <string>

#include "maze_logic.h"
#include "ui.h"

using namespace std;

namespace {

enum class LevelOutcome {
    Completed, 
    Quit     
};

MoveResult applyDirection(Maze& maze, Direction direction)
{
    switch (direction) {
    case Direction::Up:    return movePlayer(maze, -1, 0);
    case Direction::Down:  return movePlayer(maze, 1, 0);
    case Direction::Left:  return movePlayer(maze, 0, -1);
    case Direction::Right: return movePlayer(maze, 0, 1);
    default:               return MoveResult::Moved;
    }
}

LevelOutcome playLevel(Maze& maze, bool campaignMode)
{
    MoveResult lastResult = MoveResult::Moved;
    bool unknownKey = false;

    while (true) {
        clearScreen();
        showMaze(maze);
        showStatus(maze, campaignMode);

        if (unknownKey) {
            showMessage("Unknown key. Use the arrow keys, I, H or Q.");
            unknownKey = false;
        } else {
            showMoveResult(lastResult);
        }
        lastResult = MoveResult::Moved;

        if (hasPlayerWon(maze)) {
            return LevelOutcome::Completed;
        }

        Direction direction = readDirection();
        switch (direction) {
        case Direction::Quit:
            return LevelOutcome::Quit;
        case Direction::Help:
            showHelp();
            waitForEnter();
            break;
        case Direction::Inventory:
            showInventory(maze);
            waitForEnter();
            break;
        case Direction::None:
            unknownKey = true;
            break;
        default:
            lastResult = applyDirection(maze, direction);
            break;
        }
    }
}

void playFreeGame()
{
    clearScreen();
    int size = readMazeSize();

    Maze maze = {};
    if (!createMaze(maze, size)) {
        showMessage("Could not create the maze.");
        waitForEnter();
        return;
    }

    if (playLevel(maze, false) == LevelOutcome::Completed) {
        int bonus = finishLevel(maze);
        showLevelComplete(maze, bonus, false);
        waitForEnter();
    }

    destroyMaze(maze);
}

void playCampaign()
{
    Maze maze = {};
    if (!createMaze(maze, getLevelSize(1))) {
        showMessage("Could not create the maze.");
        waitForEnter();
        return;
    }

    bool finished = false;
    while (!finished) {
        if (playLevel(maze, true) == LevelOutcome::Quit) {
            destroyMaze(maze);
            return;
        }

        int bonus = finishLevel(maze);
        showLevelComplete(maze, bonus, true);

        if (maze.level >= TOTAL_LEVELS) {
            finished = true;
        } else if (maze.level == CAMPAIGN_LEVELS) {
            showBonusUnlocked();
            finished = !askYesNo("Continue to the bonus levels?");
        } else {
            waitForEnter();
        }

        if (!finished && !advanceLevel(maze, getLevelSize(maze.level + 1))) {
            showMessage("Could not create the next level.");
            finished = true;
        }
    }

    showCampaignComplete(maze);
    waitForEnter();
    destroyMaze(maze);
}

void playGraphicalGame()
{
    clearScreen();
    int size = readMazeSize();

    Maze maze = {};
    if (!createMaze(maze, size)) {
        showMessage("Could not create the maze.");
        waitForEnter();
        return;
    }
    destroyMaze(maze);
}

}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));

    bool running = true;
    while (running) {
        showMenu();
        switch (readMenuChoice()) {
        case MENU_PLAY:
            playFreeGame();
            break;
        case MENU_CAMPAIGN:
            playCampaign();
            break;
        case MENU_HELP:
            showHelp();
            waitForEnter();
            break;
        case MENU_EXIT:
            running = false;
            break;
        default:
            break;
        }
    }

    return 0;
}
