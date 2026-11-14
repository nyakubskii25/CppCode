#pragma once

void initRandom();                               // seed the random number generator
int sideOf(int count);                           // maze side length from the amount of numbers
int** createMaze(int n, int count);              // create an n x n maze
void freeMaze(int** m, int n);                   // free the memory
void printMaze(int** m, int n, int px, int py);  // draw the maze
int readKey();                                   // 0 left, 1 right, 2 down, 3 Esc
int bestMove(int** m, int n, int px, int py);    // 0 left, 1 right, 2 down, -1 no move
bool movePlayer(int** m, int n, int& px, int& py, int dir); // make a move
bool isWin(int n, int px, int py);               // check for victory
void showMenu();                                 // show the menu
void showHelp();                                 // show the help
int readSize();                                  // read the amount of numbers with validation