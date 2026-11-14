#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <windows.h>
#include "header.h"
using namespace std;

const int DX[3] = { -1, 1, 0 };   // left, right, down
const int DY[3] = { 0, 0, 1 };
const int KEYS[4] = { VK_LEFT, VK_RIGHT, VK_DOWN, VK_ESCAPE };

void initRandom() {
    srand(time(0));
}

int sideOf(int count) {
    int s = 1;
    while ((s + 1) * (s + 1) <= count) s++;      // 10 -> 3, 50 -> 7
    return s;
}

int** createMaze(int n, int count) {
    int* nums = new int[count - 1];
    for (int i = 0; i < count - 1; i++) nums[i] = i + 1;   // numbers 1..count-1
    for (int i = count - 2; i > 0; i--) swap(nums[i], nums[rand() % (i + 1)]);  // shuffle
    int** m = new int* [n];
    int k = 0;
    for (int i = 0; i < n; i++) {
        m[i] = new int[n];
        for (int j = 0; j < n; j++) m[i][j] = nums[k++ % (count - 1)];
    }
    delete[] nums;
    m[0][0] = 0;                // start
    m[n - 1][n - 1] = count;    // exit - the biggest number
    return m;
}

void freeMaze(int** m, int n) {
    for (int i = 0; i < n; i++) delete[] m[i];
    delete[] m;
}

void printMaze(int** m, int n, int px, int py) {
    system("cls");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == py && j == px) cout << setw(4) << "P";
            else if (m[i][j] < 0) cout << setw(4) << "*";   // already visited cell
            else cout << setw(4) << m[i][j];
        }
        cout << endl;
    }
}

int readKey() {
    while (true) {
        for (int d = 0; d < 4; d++) {
            if (GetAsyncKeyState(KEYS[d]) & 0x8000) {        // key is pressed
                while (GetAsyncKeyState(KEYS[d]) & 0x8000) Sleep(20);  // wait until it is released
                return d;
            }
        }
        Sleep(20);
    }
}

int bestMove(int** m, int n, int px, int py) {
    int best = -1, smallest = 100000;
    for (int d = 0; d < 3; d++) {
        int nx = px + DX[d], ny = py + DY[d];
        if (nx < 0 || nx >= n || ny >= n || m[ny][nx] < 0) continue;  // border or visited
        if (d == 0 && py == n - 1) continue;     // no moving left in the last row
        if (m[ny][nx] < smallest) {              // look for the smallest number nearby
            smallest = m[ny][nx];
            best = d;
        }
    }
    return best;
}

bool movePlayer(int** m, int n, int& px, int& py, int dir) {
    if (dir < 0 || dir != bestMove(m, n, px, py)) return false;  // not the smallest number
    m[py][px] = -1;                              // mark the cell as visited
    px += DX[dir];
    py += DY[dir];
    return true;
}

bool isWin(int n, int px, int py) {
    return px == n - 1 && py == n - 1;
}

void showMenu() {
    cout << "\n=== MAZE ===\n1. New game\n2. Help\n3. Exit\nChoice: ";
}

void showHelp() {
    cout << "\nStart: 0 (top left). Exit: the biggest number (bottom right).\n";
    cout << "Arrows: left, right, down. P - you, * - visited.\n";
    cout << "You can only step on the SMALLEST number next to you. Esc - quit.\n";
}

int readSize() {
    int n;
    cout << "How many numbers (10-50): ";
    while (!(cin >> n) || n < 10 || n > 50) {    // input validation
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Wrong! Enter a number from 10 to 50: ";
    }
    return n;
}