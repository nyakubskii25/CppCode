#include <iostream>
#include "header.h"
using namespace std;

int main() {
    initRandom();
    int choice = 0;

    while (choice != 3) {
        showMenu();
        cin >> choice;
        if (cin.fail()){ 
            cin.clear(); cin.ignore(1000, '\n'); choice = 0; // invalid menu input
        }  
        if (choice == 2) showHelp();
        if (choice == 1) {
            int count = readSize(), n = sideOf(count), px = 0, py = 0, key = 0;
            bool bad = false;
            int** maze = createMaze(n, count);

            while (!isWin(n, px, py) && key != 3) {         // 3 = Esc
                printMaze(maze, n, px, py);
                cout << "Position: " << py << "," << px << "  Value: " << maze[py][px] << endl;
                if (bad) cout << "Wall! Choose the smallest number next to you.\n";
                key = readKey();
                if (key < 3) bad = !movePlayer(maze, n, px, py, key);
            }
            printMaze(maze, n, px, py);
            if (isWin(n, px, py)) cout << "You escaped! Congratulations!\n";
            freeMaze(maze, n);
        }
    }

    return 0;
}