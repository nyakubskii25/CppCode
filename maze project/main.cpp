#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

#include "game.h"
#include "inventory.h"
#include "messagelog.h"
#include "platform.h"
#include "scores.h"
#include "ui.h"

using namespace std;

namespace {

const string SCORE_FILE = "scores.txt";

enum class LevelOutcome {
    Completed,
    Quit
};

Direction toDirection(Key key)
{
    switch (key) {
    case Key::Up: return Direction::Up;
    case Key::Down: return Direction::Down;
    case Key::Left: return Direction::Left;
    case Key::Right: return Direction::Right;
    default: return Direction::None;
    }
}

void askAndBreakWall(GameState& game)
{
    addMessage(game.log, "Break which wall? Press an arrow key.");
    showGame(game);
    Direction direction = toDirection(readKey());
    if (direction == Direction::None) {
        addMessage(game.log, "You changed your mind.");
        return;
    }
    breakWall(game, direction);
}

LevelOutcome playLevel(GameState& game)
{
    while (true) {
        showGame(game);
        if (hasWon(game)) {
            return LevelOutcome::Completed;
        }

        Key key = readKey();
        switch (key) {
        case Key::Up:
        case Key::Down:
        case Key::Left:
        case Key::Right:
            movePlayer(game, toDirection(key));
            break;
        case Key::Inventory:
            showInventory(game);
            waitForKey();
            break;
        case Key::Help:
            showHelp();
            waitForKey();
            break;
        case Key::Compass:
            useCompass(game);
            break;
        case Key::Map:
            if (useMap(game)) {
                showFullMap(game);
                waitForKey();
            }
            break;
        case Key::Break:
            askAndBreakWall(game);
            break;
        case Key::Quit:
            showGame(game);
            if (askYesNo("Leave this level and go back to the menu?")) {
                return LevelOutcome::Quit;
            }
            break;
        default:
            break;
        }
    }
}

void saveResult(ScoreTable& table, const GameState& game)
{
    string name = readPlayerName();
    addScore(table, name, game.score, game.level);
    if (!saveScores(table, SCORE_FILE)) {
        showNotice("The high score file could not be saved.");
    }
}

void playCampaign(ScoreTable& table)
{
    GameState* game = createGame(true);
    startLevel(*game, 1, 0);

    bool finished = false;
    while (!finished) {
        if (playLevel(*game) == LevelOutcome::Quit) {
            destroyGame(game);
            return;
        }

        int bonus = finishLevel(*game);
        showLevelComplete(*game, bonus);

        if (game->level >= TOTAL_LEVELS) {
            finished = true;
        } else if (game->level == CAMPAIGN_LEVELS) {
            showBonusUnlocked();
            finished = !askYesNo("Do you want to play the bonus levels?");
        }

        if (!finished) {
            startLevel(*game, game->level + 1, 0);
        }
    }

    showCampaignComplete(*game);
    saveResult(table, *game);
    destroyGame(game);
}

void playFreeGame()
{
    int size = readFreeSize();
    if (size == 0) {
        return;
    }

    GameState* game = createGame(false);
    startLevel(*game, 1, size);

    if (playLevel(*game) == LevelOutcome::Completed) {
        int bonus = finishLevel(*game);
        showLevelComplete(*game, bonus);
    }
    destroyGame(game);
}

}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    initPlatform();

    ScoreTable table = createScoreTable();
    loadScores(table, SCORE_FILE);

    bool running = true;
    while (running) {
        showMenu();
        switch (readMenuChoice()) {
        case MENU_CAMPAIGN:
            playCampaign(table);
            break;
        case MENU_FREE_PLAY:
            playFreeGame();
            break;
        case MENU_SCORES:
            showScores(table);
            waitForKey();
            break;
        case MENU_HELP:
            showHelp();
            waitForKey();
            break;
        case MENU_EXIT:
            running = false;
            break;
        default:
            break;
        }
    }

    destroyScoreTable(table);
    return 0;
}
