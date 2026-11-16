#include "ui.h"

#include <iostream>
#include <string>

#include "game.h"
#include "inventory.h"
#include "platform.h"

using namespace std;

namespace {

string paint(const string& code, const string& text)
{
    if (!colorsEnabled()) {
        return text;
    }
    return "\033[" + code + "m" + text + "\033[0m";
}

string levelLabel(const GameState& game)
{
    if (!game.campaign) {
        return "Free play";
    }
    string label = "Level " + to_string(game.level) + "/" + to_string(TOTAL_LEVELS);
    if (game.level > CAMPAIGN_LEVELS) {
        label += " (bonus)";
    }
    return label;
}

string renderCell(const GameState& game, int row, int col)
{
    const Maze& maze = game.maze;

    if (row == maze.player.row && col == maze.player.col) {
        return paint("1;92", "@ ");
    }
    if (!maze.seen[row][col]) {
        return "  ";
    }

    bool visible = isVisible(game, row, col);
    char cell = maze.cells[row][col];

    if (cell == CELL_WALL) {
        if (!colorsEnabled()) {
            return "##";
        }
        return paint(visible ? "44" : "100", "  ");
    }

    string glyph = "  ";
    string color = "0";
    switch (cell) {
    case CELL_COIN: glyph = "o "; color = "93"; break;
    case CELL_KEY: glyph = "k "; color = "96"; break;
    case CELL_PICKAXE: glyph = "p "; color = "92"; break;
    case CELL_ARTIFACT: glyph = "* "; color = "95"; break;
    case CELL_DOOR: glyph = "+ "; color = "1;33"; break;
    case CELL_LOCKED: glyph = "L "; color = "1;91"; break;
    case CELL_EXIT: glyph = "E "; color = "30;102"; break;
    default:
        if (maze.trodden[row][col]) {
            glyph = ". ";
            color = "90";
        }
        break;
    }

    if (!visible && cell != CELL_EXIT && color != "0") {
        color = "90";
    }
    if (color == "0") {
        return glyph;
    }
    return paint(color, glyph);
}

void printBanner(const string& title)
{
    string line = "  ==============================================";
    cout << paint("1;93", line) << "\n";
    cout << paint("1;97", "    " + title) << "\n";
    cout << paint("1;93", line) << "\n\n";
}

}

void showMenu()
{
    clearScreen();
    printBanner("CPPCODE MAZE  -  the maze with treasures");
    cout << "    1. Campaign  (5 levels + 3 bonus levels)\n";
    cout << "    2. Free play (choose the size of the maze)\n";
    cout << "    3. High scores\n";
    cout << "    4. Help\n";
    cout << "    5. Exit\n\n";
    cout << "  Your choice: ";
    cout.flush();
}

void showHelp()
{
    clearScreen();
    printBanner("HELP");
    cout << "  Goal: find the exit " << paint("30;102", "E ") << " of every maze.\n\n";
    cout << "  Controls\n";
    cout << "    Arrow keys or W A S D   move\n";
    cout << "    B + direction           break a wall with a pickaxe\n";
    cout << "    I                       open the inventory\n";
    cout << "    C                       compass hint (Golden Compass)\n";
    cout << "    M                       show the whole level (Ancient Map)\n";
    cout << "    Q                       leave the level\n\n";
    cout << "  Symbols\n";
    cout << "    " << paint("1;92", "@ ") << " you      " << paint("93", "o ") << " coin     "
         << paint("96", "k ") << " key      " << paint("92", "p ") << " pickaxe\n";
    cout << "    " << paint("95", "* ") << " artifact " << paint("1;33", "+ ") << " door     "
         << paint("1;91", "L ") << " locked door\n\n";
    cout << "  Doors open when you walk into them. Locked doors need a key.\n";
    cout << "  Coins give points, artifacts give powers and bonus points.\n";
    cout << "  In the dark levels you only see the cells close to you.\n";
    cout << "  Your items are kept between the levels of the campaign.\n\n";
    cout << "  Press any key to go back...";
    cout.flush();
}

void showGame(const GameState& game)
{
    const Maze& maze = game.maze;
    clearScreen();

    cout << paint("1;93", "  CPPCODE MAZE") << "   " << paint("1;97", levelLabel(game)) << "\n";
    cout << "  Moves: " << maze.moves
         << "  Score: " << game.score
         << "  Coins: " << countOf(game.inventory, ItemType::Coin)
         << "  Keys: " << countOf(game.inventory, ItemType::Key)
         << "  Pickaxes: " << countOf(game.inventory, ItemType::Pickaxe)
         << "  Artifacts: " << artifactTotal(game.inventory) << "\n";

    int rows = maze.size < VIEW_ROWS ? maze.size : VIEW_ROWS;
    int cols = maze.size < VIEW_COLS ? maze.size : VIEW_COLS;
    int top = maze.player.row - rows / 2;
    int left = maze.player.col - cols / 2;
    if (top < 0) {
        top = 0;
    }
    if (left < 0) {
        left = 0;
    }
    if (top > maze.size - rows) {
        top = maze.size - rows;
    }
    if (left > maze.size - cols) {
        left = maze.size - cols;
    }

    for (int row = top; row < top + rows; row++) {
        cout << "  ";
        for (int col = left; col < left + cols; col++) {
            cout << renderCell(game, row, col);
        }
        cout << "\n";
    }

    const MessageNode* node = game.log.head;
    int printed = 0;
    while (node != nullptr) {
        cout << "  > " << node->text << "\n";
        node = node->next;
        printed++;
    }
    while (printed < MESSAGE_LIMIT) {
        cout << "\n";
        printed++;
    }

    cout << "  Arrows/WASD move | B break | I inventory | C compass | M map | H help | Q menu\n";
    cout.flush();
}

void showInventory(const GameState& game)
{
    const Inventory& inventory = game.inventory;
    clearScreen();
    printBanner("INVENTORY");

    if (inventory.count == 0) {
        cout << "  Your bag is empty.\n";
    }

    for (int i = 0; i < inventory.count; i++) {
        const Item& item = inventory.items[i];
        switch (item.type) {
        case ItemType::Coin:
            cout << "  " << paint("93", "o ") << " Coins      x" << item.quantity << "\n";
            break;
        case ItemType::Key:
            cout << "  " << paint("96", "k ") << " Keys       x" << item.quantity << "\n";
            break;
        case ItemType::Pickaxe:
            cout << "  " << paint("92", "p ") << " Pickaxes   x" << item.quantity << "\n";
            break;
        case ItemType::Artifact:
            cout << "  " << paint("95", "* ") << " " << game.catalog[item.artifactId].name
                 << " - " << game.catalog[item.artifactId].description << "\n";
            break;
        }
    }

    cout << "\n  Bag slots in use: " << inventory.count << " of " << inventory.capacity << "\n";
    cout << "  Score: " << game.score << "\n\n";
    cout << "  Press any key to go back...";
    cout.flush();
}

void showFullMap(const GameState& game)
{
    const Maze& maze = game.maze;
    clearScreen();
    cout << paint("1;93", "  ANCIENT MAP") << "\n\n";

    for (int row = 0; row < maze.size; row++) {
        cout << "  ";
        for (int col = 0; col < maze.size; col++) {
            if (row == maze.player.row && col == maze.player.col) {
                cout << paint("1;92", "@");
                continue;
            }
            char cell = maze.cells[row][col];
            switch (cell) {
            case CELL_WALL: cout << paint("44", " "); break;
            case CELL_EXIT: cout << paint("30;102", "E"); break;
            case CELL_LOCKED: cout << paint("1;91", "L"); break;
            case CELL_DOOR: cout << paint("1;33", "+"); break;
            case CELL_COIN: cout << paint("93", "o"); break;
            case CELL_KEY: cout << paint("96", "k"); break;
            case CELL_PICKAXE: cout << paint("92", "p"); break;
            case CELL_ARTIFACT: cout << paint("95", "*"); break;
            default: cout << " "; break;
            }
        }
        cout << "\n";
    }

    cout << "\n  Press any key to go back...";
    cout.flush();
}

void showLevelComplete(const GameState& game, int bonus)
{
    clearScreen();
    printBanner("YOU ESCAPED!");
    cout << "  Moves on this level: " << game.maze.moves << "\n";
    cout << "  Level bonus:         +" << bonus << "\n";
    cout << "  Total score:         " << game.score << "\n\n";
    cout << "  Press any key to continue...";
    cout.flush();
    readKey();
}

void showBonusUnlocked()
{
    clearScreen();
    printBanner("ALL 5 LEVELS COMPLETED!");
    cout << "  Three bonus levels are now open.\n";
    cout << "  They are bigger, darker and full of locked doors.\n\n";
}

void showCampaignComplete(const GameState& game)
{
    clearScreen();
    printBanner("CAMPAIGN FINISHED");
    cout << "  Levels completed: " << game.level << "\n";
    cout << "  Total moves:      " << game.totalMoves << "\n";
    cout << "  Artifacts found:  " << artifactTotal(game.inventory) << " of " << ARTIFACT_COUNT << "\n";
    cout << "  Final score:      " << game.score << "\n\n";
}

void showScores(const ScoreTable& table)
{
    clearScreen();
    printBanner("HIGH SCORES");
    if (table.count == 0) {
        cout << "  No scores yet. Finish a campaign to be the first!\n";
    }
    for (int i = 0; i < table.count; i++) {
        cout << "  " << (i + 1) << ". " << table.entries[i].name;
        for (int space = (int)table.entries[i].name.size(); space < NAME_LIMIT + 2; space++) {
            cout << ' ';
        }
        cout << table.entries[i].score << " points, " << table.entries[i].levels << " levels\n";
    }
    cout << "\n  Press any key to go back...";
    cout.flush();
}

void showNotice(const string& text)
{
    cout << "  " << text << "\n";
    cout.flush();
}

void waitForKey()
{
    readKey();
}

bool askYesNo(const string& question)
{
    cout << "\n  " << question << " (y/n) ";
    cout.flush();
    while (true) {
        Key key = readKey();
        if (key == Key::Yes) {
            return true;
        }
        if (key == Key::No || key == Key::Quit || key == Key::Escape) {
            return false;
        }
    }
}

int readMenuChoice()
{
    string line;
    if (!getline(cin, line)) {
        return MENU_EXIT;
    }
    if (line.size() != 1 || line[0] < '1' || line[0] > '5') {
        return 0;
    }
    return line[0] - '0';
}

int readFreeSize()
{
    while (true) {
        clearScreen();
        printBanner("FREE PLAY");
        cout << "  Enter the size of the maze from " << MIN_FREE_SIZE << " to " << MAX_FREE_SIZE << ".\n";
        cout << "  (an empty line goes back)\n\n  Size: ";
        cout.flush();

        string line;
        if (!getline(cin, line) || line.empty()) {
            return 0;
        }

        bool digits = line.size() <= 3;
        for (size_t i = 0; i < line.size(); i++) {
            if (line[i] < '0' || line[i] > '9') {
                digits = false;
            }
        }

        if (digits) {
            int value = 0;
            for (size_t i = 0; i < line.size(); i++) {
                value = value * 10 + (line[i] - '0');
            }
            if (value >= MIN_FREE_SIZE && value <= MAX_FREE_SIZE) {
                return value;
            }
        }

        cout << "\n  Wrong size. Press Enter to try again...";
        cout.flush();
        string ignored;
        if (!getline(cin, ignored)) {
            return 0;
        }
    }
}

string readPlayerName()
{
    cout << "  Enter your name for the high scores: ";
    cout.flush();
    string line;
    getline(cin, line);
    return line;
}
