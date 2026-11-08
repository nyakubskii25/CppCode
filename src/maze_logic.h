#ifndef MAZE_LOGIC_H
#define MAZE_LOGIC_H

// ---------------------------------------------------------------
// Game logic interface (BACKEND part).
// Functions declared here must NOT use cin / cout.
// The frontend (ui.h / ui.cpp / gfx.cpp / main.cpp) only relies on
// this header.
// ---------------------------------------------------------------

const int MIN_MAZE_SIZE = 10;
const int MAX_MAZE_SIZE = 50;

// Campaign: 5 normal levels, then 3 bonus levels.
const int CAMPAIGN_LEVELS = 5;
const int BONUS_LEVELS = 3;
const int TOTAL_LEVELS = CAMPAIGN_LEVELS + BONUS_LEVELS;

// Cell symbols stored in Maze::cells
const char CELL_WALL = '#';
const char CELL_PATH = ' ';
const char CELL_DOOR = 'D';      // special door: free shortcut through a wall
const char CELL_LOCKED = 'L';    // locked door: shortcut that needs a key
const char CELL_EXIT = 'E';
const char CELL_COIN = '$';
const char CELL_KEY = 'K';
const char CELL_PICKAXE = 'X';   // breaks one wall
const char CELL_ARTIFACT = 'A';  // rare treasure

// Points
const int COIN_POINTS = 10;
const int ARTIFACT_POINTS = 100;

struct Inventory {
    int coins;
    int keys;
    int pickaxes;
    int artifacts;
};

struct Maze {
    int size;            // maze is size x size
    char** cells;        // dynamic 2D array [row][col]
    int playerRow;
    int playerCol;
    int moves;           // successful moves on the current level
    int level;           // current level, starting from 1
    int score;
    Inventory inventory; // kept between levels in the campaign
};

// What happened when the player tried to move.
enum class MoveResult {
    Blocked,            // wall (and no pickaxe) or outside the maze
    LockedDoor,         // locked door and no key
    Moved,              // plain move
    CollectedCoin,
    CollectedKey,
    CollectedPickaxe,
    CollectedArtifact,
    OpenedLockedDoor,   // a key was used
    BrokeWall           // a pickaxe was used
};

// Returns true if size is in range MIN_MAZE_SIZE..MAX_MAZE_SIZE.
bool isValidMazeSize(int size);

// Maze size for a campaign level (1..TOTAL_LEVELS), 0 if invalid.
int getLevelSize(int level);

// Returns true for the levels after CAMPAIGN_LEVELS.
bool isBonusLevel(int level);

// Allocates the dynamic array and generates a new random maze with
// items. Starts at level 1 with an empty inventory and zero score.
bool createMaze(Maze& maze, int size);

// Generates the next level: new maze, but the score and the
// inventory are kept and the level number is increased.
bool advanceLevel(Maze& maze, int newSize);

// Frees the dynamic memory.
void destroyMaze(Maze& maze);

// Moves the player and collects / uses items automatically.
MoveResult movePlayer(Maze& maze, int rowDelta, int colDelta);

// Returns true if the player stands on the exit.
bool hasPlayerWon(const Maze& maze);

// Adds the level bonus to the score and returns the bonus.
int finishLevel(Maze& maze);

#endif
