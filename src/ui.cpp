#include "ui.h"

#include <cctype>
#include <cstdlib>
#include <iostream>

#include <conio.h>
#include <windows.h>

using namespace std;

namespace {

// Windows console colors
const WORD COLOR_DEFAULT = 7;
const WORD COLOR_WALL = 8;
const WORD COLOR_PLAYER = 10;
const WORD COLOR_DOOR = 9;
const WORD COLOR_LOCKED = 12;
const WORD COLOR_EXIT = 14;
const WORD COLOR_COIN = 6;
const WORD COLOR_KEY = 11;
const WORD COLOR_PICKAXE = 13;
const WORD COLOR_ARTIFACT = 15;
const WORD COLOR_TITLE = 13;
const WORD COLOR_ERROR = 12;
const WORD COLOR_SUCCESS = 10;

// Key codes returned by _getch()
const int KEY_EXTENDED_A = 0;
const int KEY_EXTENDED_B = 224;
const int KEY_ARROW_UP = 72;
const int KEY_ARROW_DOWN = 80;
const int KEY_ARROW_LEFT = 75;
const int KEY_ARROW_RIGHT = 77;

const int MAX_INPUT_DIGITS = 4;

void setColor(WORD color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Reads a whole line; returns false if the input stream is closed.
bool readLine(string& line)
{
    if (!getline(cin, line)) {
        cin.clear();
        line.clear();
        return false;
    }
    return true;
}

// Removes spaces and tabs from both ends of the text.
string trimText(const string& text)
{
    size_t first = text.find_first_not_of(" \t");
    if (first == string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t");
    return text.substr(first, last - first + 1);
}

// Converts text to a number. Rejects empty text, letters, signs and
// very long input, so the user cannot break the program.
bool parseInteger(const string& text, int& value)
{
    string trimmed = trimText(text);
    if (trimmed.empty()
        || trimmed.size() > static_cast<size_t>(MAX_INPUT_DIGITS)) {
        return false;
    }
    for (size_t i = 0; i < trimmed.size(); i++) {
        if (!isdigit(static_cast<unsigned char>(trimmed[i]))) {
            return false;
        }
    }

    value = atoi(trimmed.c_str());
    return true;
}

void printColored(WORD color, const string& text)
{
    setColor(color);
    cout << text << "\n";
    setColor(COLOR_DEFAULT);
}

void printError(const string& message)
{
    printColored(COLOR_ERROR, message);
}

// One line of the legend: a colored symbol and its description.
void printLegendLine(WORD color, const string& symbol,
                     const string& description)
{
    setColor(color);
    cout << "  " << symbol;
    setColor(COLOR_DEFAULT);
    cout << "  " << description << "\n";
}

// Draws one cell as two characters, so the maze looks roughly square.
void drawCell(const Maze& maze, int row, int col)
{
    if (row == maze.playerRow && col == maze.playerCol) {
        setColor(COLOR_PLAYER);
        cout << "@ ";
        return;
    }

    switch (maze.cells[row][col]) {
    case CELL_WALL:
        setColor(COLOR_WALL);
        cout << "##";
        break;
    case CELL_DOOR:
        setColor(COLOR_DOOR);
        cout << "D ";
        break;
    case CELL_LOCKED:
        setColor(COLOR_LOCKED);
        cout << "L ";
        break;
    case CELL_EXIT:
        setColor(COLOR_EXIT);
        cout << "E ";
        break;
    case CELL_COIN:
        setColor(COLOR_COIN);
        cout << "$ ";
        break;
    case CELL_KEY:
        setColor(COLOR_KEY);
        cout << "K ";
        break;
    case CELL_PICKAXE:
        setColor(COLOR_PICKAXE);
        cout << "X ";
        break;
    case CELL_ARTIFACT:
        setColor(COLOR_ARTIFACT);
        cout << "A ";
        break;
    default:
        setColor(COLOR_DEFAULT);
        cout << "  ";
        break;
    }
}

void printLevelName(const Maze& maze)
{
    if (isBonusLevel(maze.level)) {
        cout << "Bonus level " << (maze.level - CAMPAIGN_LEVELS) << "/"
             << BONUS_LEVELS;
    } else {
        cout << "Level " << maze.level << "/" << CAMPAIGN_LEVELS;
    }
}

} // namespace

// ---------------------------------------------------------------
// Screens
// ---------------------------------------------------------------

void clearScreen()
{
    system("cls");
}

void showMenu()
{
    clearScreen();
    setColor(COLOR_TITLE);
    cout << "=============================\n";
    cout << "          M A Z E            \n";
    cout << "=============================\n\n";
    setColor(COLOR_DEFAULT);
    cout << "Find the way from the top-left corner to the exit (E)\n";
    cout << "in the bottom-right corner. Collect items on the way!\n\n";
    cout << "  " << MENU_PLAY << ". Free play (choose the size)\n";
    cout << "  " << MENU_CAMPAIGN << ". Campaign ("
         << CAMPAIGN_LEVELS << " levels + " << BONUS_LEVELS
         << " bonus levels)\n";
    cout << "  " << MENU_PLAY_GRAPHICS << ". Play (graphics)\n";
    cout << "  " << MENU_HELP << ". Help\n";
    cout << "  " << MENU_EXIT << ". Exit\n\n";
}

void showHelp()
{
    clearScreen();
    setColor(COLOR_TITLE);
    cout << "=========== HELP ===========\n\n";
    setColor(COLOR_DEFAULT);
    cout << "Goal:\n";
    cout << "  Start at the top-left corner and reach the exit in\n";
    cout << "  the bottom-right corner.\n\n";
    cout << "Controls:\n";
    cout << "  Arrow keys (or W, A, S, D)  move the player\n";
    cout << "  I                           show the inventory\n";
    cout << "  H                           show this help\n";
    cout << "  Q                           back to the main menu\n\n";

    cout << "Legend:\n";
    printLegendLine(COLOR_PLAYER, "@ ", "you");
    printLegendLine(COLOR_WALL, "##", "wall (needs a pickaxe to break it)");
    printLegendLine(COLOR_DOOR, "D ", "special door (a free shortcut)");
    printLegendLine(COLOR_LOCKED, "L ", "locked door (a shortcut, needs a key)");
    printLegendLine(COLOR_EXIT, "E ", "exit");
    printLegendLine(COLOR_COIN, "$ ", "coin (+" + to_string(COIN_POINTS)
                                     + " points)");
    printLegendLine(COLOR_KEY, "K ", "key (opens one locked door)");
    printLegendLine(COLOR_PICKAXE, "X ", "pickaxe (breaks one wall)");
    printLegendLine(COLOR_ARTIFACT, "A ", "artifact (+"
                                         + to_string(ARTIFACT_POINTS)
                                         + " points)");

    cout << "\nItems are collected automatically when you step on them.\n";
    cout << "To break a wall, walk into it while you have a pickaxe.\n\n";
    cout << "Free play: the maze size is " << MIN_MAZE_SIZE << "x"
         << MIN_MAZE_SIZE << " to " << MAX_MAZE_SIZE << "x" << MAX_MAZE_SIZE
         << ".\n";
    cout << "Campaign: " << CAMPAIGN_LEVELS
         << " levels of growing size. Your score and inventory are\n";
    cout << "kept between levels. After the last level you can play "
         << BONUS_LEVELS << " bonus levels.\n";
}

void showMaze(const Maze& maze)
{
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            drawCell(maze, row, col);
        }
        setColor(COLOR_DEFAULT);
        cout << "\n";
    }
    setColor(COLOR_DEFAULT);
}

void showStatus(const Maze& maze, bool showLevel)
{
    cout << "\n";
    if (showLevel) {
        printLevelName(maze);
        cout << " | ";
    }
    cout << "Score: " << maze.score << " | Moves: " << maze.moves
         << " | Position: (" << maze.playerRow << ", " << maze.playerCol
         << ")\n";
    cout << "Coins: " << maze.inventory.coins
         << " | Keys: " << maze.inventory.keys
         << " | Pickaxes: " << maze.inventory.pickaxes
         << " | Artifacts: " << maze.inventory.artifacts << "\n";
    cout << "Arrows/WASD - move | I - inventory | H - help | Q - menu\n";
}

void showInventory(const Maze& maze)
{
    clearScreen();
    setColor(COLOR_TITLE);
    cout << "=========== INVENTORY ===========\n\n";
    setColor(COLOR_DEFAULT);

    cout << "  $  Coins:     " << maze.inventory.coins << "   (+"
         << COIN_POINTS << " points each)\n";
    cout << "  K  Keys:      " << maze.inventory.keys
         << "   (each opens one locked door)\n";
    cout << "  X  Pickaxes:  " << maze.inventory.pickaxes
         << "   (each breaks one wall)\n";
    cout << "  A  Artifacts: " << maze.inventory.artifacts << "   (+"
         << ARTIFACT_POINTS << " points each)\n\n";
    cout << "Total score: " << maze.score << "\n";
}

void showMessage(const string& message)
{
    printError(message);
}

void showMoveResult(MoveResult result)
{
    switch (result) {
    case MoveResult::Blocked:
        printError("You cannot go that way!");
        break;
    case MoveResult::LockedDoor:
        printError("The door is locked. You need a key (K).");
        break;
    case MoveResult::CollectedCoin:
        printColored(COLOR_SUCCESS, "You picked up a coin! (+"
                                    + to_string(COIN_POINTS) + " points)");
        break;
    case MoveResult::CollectedKey:
        printColored(COLOR_SUCCESS, "You found a key!");
        break;
    case MoveResult::CollectedPickaxe:
        printColored(COLOR_SUCCESS,
                     "You found a pickaxe! Walk into a wall to break it.");
        break;
    case MoveResult::CollectedArtifact:
        printColored(COLOR_SUCCESS, "You found an ancient artifact! (+"
                                    + to_string(ARTIFACT_POINTS)
                                    + " points)");
        break;
    case MoveResult::OpenedLockedDoor:
        printColored(COLOR_SUCCESS, "You unlocked the door with a key.");
        break;
    case MoveResult::BrokeWall:
        printColored(COLOR_SUCCESS, "You smashed the wall with a pickaxe!");
        break;
    case MoveResult::Moved:
        break;
    }
}

void showLevelComplete(const Maze& maze, int bonus, bool showLevel)
{
    setColor(COLOR_EXIT);
    if (showLevel) {
        cout << "\n*** ";
        printLevelName(maze);
        cout << " complete! ***\n";
    } else {
        cout << "\n*** Congratulations! You escaped the maze! ***\n";
    }
    setColor(COLOR_DEFAULT);
    cout << "Moves: " << maze.moves << " | Level bonus: +" << bonus
         << " | Total score: " << maze.score << "\n";
}

void showBonusUnlocked()
{
    printColored(COLOR_ARTIFACT, "\nYou finished all " + to_string(CAMPAIGN_LEVELS)
                                 + " levels! " + to_string(BONUS_LEVELS)
                                 + " bonus levels are unlocked.");
    cout << "The bonus mazes are bigger and hide more treasure.\n\n";
}

void showCampaignComplete(const Maze& maze)
{
    clearScreen();
    setColor(COLOR_TITLE);
    cout << "========== CAMPAIGN RESULT ==========\n\n";
    setColor(COLOR_DEFAULT);
    cout << "Levels completed: " << maze.level << "/" << TOTAL_LEVELS << "\n";
    cout << "Coins collected:  " << maze.inventory.coins << "\n";
    cout << "Artifacts found:  " << maze.inventory.artifacts << "\n";
    cout << "Final score:      " << maze.score << "\n";
}

void waitForEnter()
{
    cout << "\nPress Enter to continue...";
    string ignored;
    readLine(ignored);
}

// ---------------------------------------------------------------
// Input with validation
// ---------------------------------------------------------------

int readMenuChoice()
{
    while (true) {
        cout << "Choose an option (" << MENU_PLAY << "-" << MENU_EXIT
             << "): ";
        string line;
        int choice = 0;
        if (!readLine(line)) {
            return MENU_EXIT;  // input closed, leave the program
        }
        if (parseInteger(line, choice)
            && choice >= MENU_PLAY && choice <= MENU_EXIT) {
            return choice;
        }
        printError("Invalid choice. Please enter a number from the menu.");
    }
}

int readMazeSize()
{
    while (true) {
        cout << "Enter maze size (" << MIN_MAZE_SIZE << "-"
             << MAX_MAZE_SIZE << "): ";
        string line;
        int size = 0;
        if (!readLine(line)) {
            return MIN_MAZE_SIZE;
        }
        if (parseInteger(line, size) && isValidMazeSize(size)) {
            return size;
        }
        printError("Invalid size. Enter a whole number in the given range.");
    }
}

bool askYesNo(const string& question)
{
    while (true) {
        cout << question << " (y/n): ";
        string line;
        if (!readLine(line)) {
            return false;
        }

        string answer = trimText(line);
        if (answer.size() == 1) {
            char letter = static_cast<char>(
                tolower(static_cast<unsigned char>(answer[0])));
            if (letter == 'y') {
                return true;
            }
            if (letter == 'n') {
                return false;
            }
        }
        printError("Please answer with y or n.");
    }
}

Direction readDirection()
{
    int key = _getch();

    // Arrow keys send two codes: a prefix (0 or 224) and the key code.
    if (key == KEY_EXTENDED_A || key == KEY_EXTENDED_B) {
        key = _getch();
        switch (key) {
        case KEY_ARROW_UP:    return Direction::Up;
        case KEY_ARROW_DOWN:  return Direction::Down;
        case KEY_ARROW_LEFT:  return Direction::Left;
        case KEY_ARROW_RIGHT: return Direction::Right;
        default:              return Direction::None;
        }
    }

    switch (tolower(key)) {
    case 'w': return Direction::Up;
    case 's': return Direction::Down;
    case 'a': return Direction::Left;
    case 'd': return Direction::Right;
    case 'i': return Direction::Inventory;
    case 'h': return Direction::Help;
    case 'q': return Direction::Quit;
    default:  return Direction::None;
    }
}
