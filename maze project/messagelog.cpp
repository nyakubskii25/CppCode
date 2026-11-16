#include "messagelog.h"

using namespace std;

MessageLog createLog()
{
    MessageLog log;
    log.head = nullptr;
    log.tail = nullptr;
    log.count = 0;
    return log;
}

void addMessage(MessageLog& log, const string& text)
{
    MessageNode* node = new MessageNode;
    node->text = text;
    node->next = nullptr;

    if (log.tail == nullptr) {
        log.head = node;
    } else {
        log.tail->next = node;
    }
    log.tail = node;
    log.count++;

    while (log.count > MESSAGE_LIMIT) {
        MessageNode* oldest = log.head;
        log.head = oldest->next;
        delete oldest;
        log.count--;
    }
}

void clearLog(MessageLog& log)
{
    while (log.head != nullptr) {
        MessageNode* node = log.head;
        log.head = node->next;
        delete node;
    }
    log.tail = nullptr;
    log.count = 0;
}
