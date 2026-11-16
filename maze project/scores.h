#pragma once

#include <string>

#include "types.h"

ScoreTable createScoreTable();
void destroyScoreTable(ScoreTable& table);
void loadScores(ScoreTable& table, const std::string& fileName);
bool saveScores(const ScoreTable& table, const std::string& fileName);
void addScore(ScoreTable& table, const std::string& name, int score, int levels);
std::string cleanName(const std::string& rawName);
