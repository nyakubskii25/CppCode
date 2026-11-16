#include "scores.h"

#include <fstream>

using namespace std;

namespace {

const int START_CAPACITY = 4;

void growTable(ScoreTable& table)
{
    int newCapacity = table.capacity * 2;
    ScoreEntry* bigger = new ScoreEntry[newCapacity];
    for (int i = 0; i < table.count; i++) {
        bigger[i] = table.entries[i];
    }
    delete[] table.entries;
    table.entries = bigger;
    table.capacity = newCapacity;
}

}

ScoreTable createScoreTable()
{
    ScoreTable table;
    table.capacity = START_CAPACITY;
    table.count = 0;
    table.entries = new ScoreEntry[table.capacity];
    return table;
}

void destroyScoreTable(ScoreTable& table)
{
    delete[] table.entries;
    table.entries = nullptr;
    table.count = 0;
    table.capacity = 0;
}

string cleanName(const string& rawName)
{
    size_t first = rawName.find_first_not_of(" \t");
    size_t last = rawName.find_last_not_of(" \t");
    string trimmed = first == string::npos ? "" : rawName.substr(first, last - first + 1);

    string name;
    for (size_t i = 0; i < trimmed.size() && (int)name.size() < NAME_LIMIT; i++) {
        char letter = trimmed[i];
        if (letter == ' ' || letter == '\t') {
            letter = '_';
        }
        if (letter >= 33 && letter <= 126) {
            name += letter;
        } else if (letter == '_') {
            name += letter;
        }
    }
    if (name.empty()) {
        name = "Player";
    }
    return name;
}

void addScore(ScoreTable& table, const string& name, int score, int levels)
{
    if (table.count == table.capacity) {
        growTable(table);
    }

    int position = table.count;
    while (position > 0 && table.entries[position - 1].score < score) {
        table.entries[position] = table.entries[position - 1];
        position--;
    }

    table.entries[position].name = cleanName(name);
    table.entries[position].score = score;
    table.entries[position].levels = levels;
    table.count++;

    if (table.count > MAX_SCORES) {
        table.count = MAX_SCORES;
    }
}

void loadScores(ScoreTable& table, const string& fileName)
{
    ifstream input(fileName);
    if (!input) {
        return;
    }

    string name;
    int score = 0;
    int levels = 0;
    while (input >> name >> score >> levels) {
        addScore(table, name, score, levels);
    }
}

bool saveScores(const ScoreTable& table, const string& fileName)
{
    ofstream output(fileName);
    if (!output) {
        return false;
    }
    for (int i = 0; i < table.count; i++) {
        output << table.entries[i].name << ' ' << table.entries[i].score
               << ' ' << table.entries[i].levels << '\n';
    }
    return true;
}
