#pragma once

template <typename T>
T** createGrid(int size, T value)
{
    T** grid = new T*[size];
    for (int row = 0; row < size; row++) {
        grid[row] = new T[size];
        for (int col = 0; col < size; col++) {
            grid[row][col] = value;
        }
    }
    return grid;
}

template <typename T>
void destroyGrid(T**& grid, int size)
{
    if (grid == nullptr) {
        return;
    }
    for (int row = 0; row < size; row++) {
        delete[] grid[row];
    }
    delete[] grid;
    grid = nullptr;
}
