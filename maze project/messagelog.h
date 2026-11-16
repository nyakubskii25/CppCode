#pragma once

#include <string>

#include "types.h"

MessageLog createLog();
void addMessage(MessageLog& log, const std::string& text);
void clearLog(MessageLog& log);
