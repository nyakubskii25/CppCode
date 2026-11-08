#include "gfx.h"

#include "raylib.h"

namespace {

const int MAZE_AREA_PIXELS = 800;   // maximum width and height of the maze
const int HEADER_HEIGHT = 40;
const int FONT_SIZE = 20;
const int HEADER_FONT_SIZE = 18;
const int TITLE_FONT_SIZE = 48;
const int TARGET_FPS = 60;

const Color COLOR_BACKGROUND = { 24, 26, 36, 255 };
const Color COLOR_WALL = { 52, 58, 84, 255 };
const Color COLOR_PATH = { 236, 232, 214, 255 };
const Color COLOR_DOOR = { 90, 140, 230, 255 };
const Color COLOR_LOCKED = { 220, 70, 70, 255 };
const Color COLOR_EXIT = { 250, 200, 40, 255 };
const Color COLOR_PLAYER = { 60, 190, 90, 255 };
const Color COLOR_TEXT = { 240, 240, 240, 255 };

const Color COLOR_COIN = { 230, 170, 20, 255 };
const Color COLOR_KEY = { 60, 200, 220, 255 };
const Color COLOR_PICKAXE = { 170, 80, 200, 255 };
const Color COLOR_ARTIFACT = { 255, 120, 170, 255 };

bool isKeyActivated(int key)
{
    return IsKeyPressed(key) || IsKeyPressedRepeat(key);
}

// Reads the arrow keys / WASD and moves the player.
// Items are collected by movePlayer itself.
void handleInput(Maze& maze)
{
    if (isKeyActivated(KEY_UP) || isKeyActivated(KEY_W)) {
        movePlayer(maze, -1, 0);
    } else if (isKeyActivated(KEY_DOWN) || isKeyActivated(KEY_S)) {
        movePlayer(maze, 1, 0);
    } else if (isKeyActivated(KEY_LEFT) || isKeyActivated(KEY_A)) {
        movePlayer(maze, 0, -1);
    } else if (isKeyActivated(KEY_RIGHT) || isKeyActivated(KEY_D)) {
        movePlayer(maze, 0, 1);
    }
}

Color getCellColor(char cell)
{
    switch (cell) {
    case CELL_WALL:   return COLOR_WALL;
    case CELL_DOOR:   return COLOR_DOOR;
    case CELL_LOCKED: return COLOR_LOCKED;
    case CELL_EXIT:   return COLOR_EXIT;
    default:          return COLOR_PATH;   // path and items
    }
}

// Returns true and sets the color if the cell holds an item.
bool getItemColor(char cell, Color& color)
{
    switch (cell) {
    case CELL_COIN:     color = COLOR_COIN;     return true;
    case CELL_KEY:      color = COLOR_KEY;      return true;
    case CELL_PICKAXE:  color = COLOR_PICKAXE;  return true;
    case CELL_ARTIFACT: color = COLOR_ARTIFACT; return true;
    default:            return false;
    }
}

void drawMaze(const Maze& maze, int cellSize)
{
    for (int row = 0; row < maze.size; row++) {
        for (int col = 0; col < maze.size; col++) {
            char cell = maze.cells[row][col];
            int x = col * cellSize;
            int y = HEADER_HEIGHT + row * cellSize;
            DrawRectangle(x, y, cellSize, cellSize, getCellColor(cell));

            // Items are small circles in the middle of the cell.
            Color itemColor = COLOR_PATH;
            if (getItemColor(cell, itemColor)) {
                DrawCircle(x + cellSize / 2, y + cellSize / 2,
                           static_cast<float>(cellSize) * 0.25f, itemColor);
            }
        }
    }

    // The player is a bigger circle in the middle of its cell.
    int centerX = maze.playerCol * cellSize + cellSize / 2;
    int centerY = HEADER_HEIGHT + maze.playerRow * cellSize + cellSize / 2;
    DrawCircle(centerX, centerY, static_cast<float>(cellSize) * 0.38f,
               COLOR_PLAYER);
}

void drawHeader(const Maze& maze, int windowWidth)
{
    DrawRectangle(0, 0, windowWidth, HEADER_HEIGHT, COLOR_BACKGROUND);
    DrawText(TextFormat("Moves: %d | Score: %d | Keys: %d | Pickaxes: %d",
                        maze.moves, maze.score, maze.inventory.keys,
                        maze.inventory.pickaxes),
             10, 10, HEADER_FONT_SIZE, COLOR_TEXT);

    const char* hint = "Esc - menu";
    int hintWidth = MeasureText(hint, HEADER_FONT_SIZE);
    DrawText(hint, windowWidth - hintWidth - 10, 10, HEADER_FONT_SIZE,
             COLOR_TEXT);
}

void drawWinOverlay(const Maze& maze, int windowWidth, int windowHeight)
{
    DrawRectangle(0, 0, windowWidth, windowHeight, Fade(BLACK, 0.7f));

    const char* title = "YOU ESCAPED!";
    int titleWidth = MeasureText(title, TITLE_FONT_SIZE);
    DrawText(title, (windowWidth - titleWidth) / 2, windowHeight / 2 - 50,
             TITLE_FONT_SIZE, COLOR_EXIT);

    const char* info = TextFormat("Moves: %d | Score: %d   (press Enter)",
                                  maze.moves, maze.score);
    int infoWidth = MeasureText(info, FONT_SIZE);
    DrawText(info, (windowWidth - infoWidth) / 2, windowHeight / 2 + 20,
             FONT_SIZE, COLOR_TEXT);
}

} // namespace

void runGraphicalGame(Maze& maze)
{
    int cellSize = MAZE_AREA_PIXELS / maze.size;
    int windowWidth = cellSize * maze.size;
    int windowHeight = windowWidth + HEADER_HEIGHT;

    InitWindow(windowWidth, windowHeight, "Maze");
    SetTargetFPS(TARGET_FPS);

    bool finished = false;
    bool bonusAdded = false;
    while (!WindowShouldClose() && !finished) {
        bool won = hasPlayerWon(maze);
        if (!won) {
            handleInput(maze);
        } else {
            if (!bonusAdded) {
                finishLevel(maze);  // add the level bonus exactly once
                bonusAdded = true;
            }
            if (IsKeyPressed(KEY_ENTER)) {
                finished = true;
            }
        }

        BeginDrawing();
        ClearBackground(COLOR_BACKGROUND);
        drawMaze(maze, cellSize);
        drawHeader(maze, windowWidth);
        if (won) {
            drawWinOverlay(maze, windowWidth, windowHeight);
        }
        EndDrawing();
    }

    CloseWindow();
}
