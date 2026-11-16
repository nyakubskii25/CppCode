#pragma once

#include <string>

const char CELL_WALL = '#';
const char CELL_PATH = ' ';
const char CELL_DOOR = 'D';
const char CELL_LOCKED = 'L';
const char CELL_COIN = 'C';
const char CELL_KEY = 'K';
const char CELL_PICKAXE = 'P';
const char CELL_ARTIFACT = 'A';
const char CELL_EXIT = 'E';

const int MIN_FREE_SIZE = 10;
const int MAX_FREE_SIZE = 49;
const int CAMPAIGN_LEVELS = 5;
const int TOTAL_LEVELS = 8;

const int COIN_POINTS = 10;
const int ARTIFACT_POINTS = 100;
const int KEY_BONUS_POINTS = 5;
const int MIN_LEVEL_BONUS = 50;
const int LEVEL_BONUS_PER_CELL = 10;

const int NO_FOG = -1;
const int LENS_BONUS = 3;
const int MESSAGE_LIMIT = 5;
const int MAX_SCORES = 10;
const int NAME_LIMIT = 12;
const int VIEW_ROWS = 15;
const int VIEW_COLS = 35;

struct Position {
    int row;
    int col;
};

enum class Direction {
    None,
    Up,
    Down,
    Left,
    Right
};

enum class Key {
    None,
    Up,
    Down,
    Left,
    Right,
    Enter,
    Escape,
    Inventory,
    Help,
    Compass,
    Map,
    Break,
    Quit,
    Yes,
    No
};

enum class MoveResult {
    Moved,
    Blocked,
    OpenedDoor,
    OpenedLockedDoor,
    LockedDoor,
    CollectedCoin,
    CollectedKey,
    CollectedPickaxe,
    CollectedArtifact
};

enum class ItemType {
    Coin,
    Key,
    Pickaxe,
    Artifact
};

enum ArtifactId {
    ARTIFACT_LENS = 0,
    ARTIFACT_MAP = 1,
    ARTIFACT_COMPASS = 2,
    ARTIFACT_RING = 3,
    ARTIFACT_COUNT = 4
};

struct ArtifactInfo {
    int id;
    std::string name;
    std::string description;
};

struct Item {
    ItemType type;
    int artifactId;
    int quantity;
};

struct Inventory {
    Item* items;
    int count;
    int capacity;
};

struct MessageNode {
    std::string text;
    MessageNode* next;
};

struct MessageLog {
    MessageNode* head;
    MessageNode* tail;
    int count;
};

struct ScoreEntry {
    std::string name;
    int score;
    int levels;
};

struct ScoreTable {
    ScoreEntry* entries;
    int count;
    int capacity;
};

struct LevelConfig {
    int size;
    int fogRadius;
    bool gateDoor;
    int doors;
    int lockedDoors;
    int coins;
    int pickaxes;
    int artifacts;
    bool bonus;
};

struct Maze {
    int size;
    char** cells;
    bool** seen;
    bool** trodden;
    Position player;
    Position start;
    Position exit;
    int fogRadius;
    int moves;
    bool mapUsed;
};

struct GameState {
    Maze maze;
    Inventory inventory;
    ArtifactInfo* catalog;
    int catalogCount;
    MessageLog log;
    int level;
    int score;
    int totalMoves;
    bool campaign;
};
