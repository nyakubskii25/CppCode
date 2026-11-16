#include "platform.h"

#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <conio.h>
#else
#include <poll.h>
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

namespace {

bool colors = false;

Key translateCharacter(int character)
{
    switch (character) {
    case 'w': case 'W': return Key::Up;
    case 's': case 'S': return Key::Down;
    case 'a': case 'A': return Key::Left;
    case 'd': case 'D': return Key::Right;
    case 'i': case 'I': return Key::Inventory;
    case 'h': case 'H': return Key::Help;
    case 'c': case 'C': return Key::Compass;
    case 'm': case 'M': return Key::Map;
    case 'b': case 'B': return Key::Break;
    case 'q': case 'Q': return Key::Quit;
    case 'y': case 'Y': return Key::Yes;
    case 'n': case 'N': return Key::No;
    case 13: case 10: return Key::Enter;
    case 27: return Key::Escape;
    default: return Key::None;
    }
}

}

#ifdef _WIN32

void initPlatform()
{
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode)) {
        colors = SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    }
}

bool colorsEnabled()
{
    return colors;
}

void clearScreen()
{
    if (colors) {
        cout << "\033[H\033[2J\033[3J";
        cout.flush();
    } else {
        system("cls");
    }
}

Key readKey()
{
    int character = _getch();
    if (character == 0 || character == 224) {
        int code = _getch();
        switch (code) {
        case 72: return Key::Up;
        case 80: return Key::Down;
        case 75: return Key::Left;
        case 77: return Key::Right;
        default: return Key::None;
        }
    }
    return translateCharacter(character);
}

#else

namespace {

int readByte()
{
    unsigned char value = 0;
    ssize_t amount = read(STDIN_FILENO, &value, 1);
    if (amount <= 0) {
        return -1;
    }
    return value;
}

bool byteWaiting()
{
    pollfd descriptor;
    descriptor.fd = STDIN_FILENO;
    descriptor.events = POLLIN;
    descriptor.revents = 0;
    return poll(&descriptor, 1, 40) > 0;
}

}

void initPlatform()
{
    colors = true;
}

bool colorsEnabled()
{
    return colors;
}

void clearScreen()
{
    cout << "\033[H\033[2J\033[3J";
    cout.flush();
}

Key readKey()
{
    termios saved;
    bool terminal = tcgetattr(STDIN_FILENO, &saved) == 0;
    if (terminal) {
        termios raw = saved;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    Key result = Key::None;
    int character = readByte();
    if (character < 0) {
        result = Key::Quit;
    } else if (character == 27) {
        result = Key::Escape;
        if (byteWaiting()) {
            int second = readByte();
            if (second == '[' && byteWaiting()) {
                int third = readByte();
                switch (third) {
                case 'A': result = Key::Up; break;
                case 'B': result = Key::Down; break;
                case 'D': result = Key::Left; break;
                case 'C': result = Key::Right; break;
                default: result = Key::None; break;
                }
            }
        }
    } else {
        result = translateCharacter(character);
    }

    if (terminal) {
        tcsetattr(STDIN_FILENO, TCSANOW, &saved);
    }
    return result;
}

#endif
