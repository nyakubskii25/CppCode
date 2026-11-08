#ifndef GFX_H
#define GFX_H

#include "maze_logic.h"

// ---------------------------------------------------------------
// Optional graphical mode (raylib).
// Opens a window, draws the maze and handles the arrow keys.
// Returns when the player wins and presses Enter, or closes the
// window / presses Esc.
// NOTE: do not include windows.h in gfx.cpp - it conflicts with raylib.
// ---------------------------------------------------------------
void runGraphicalGame(Maze& maze);

#endif
