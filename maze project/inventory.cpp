#include "inventory.h"

using namespace std;

namespace {

const int START_CAPACITY = 2;

void growInventory(Inventory& inventory)
{
    int newCapacity = inventory.capacity * 2;
    Item* bigger = new Item[newCapacity];
    for (int i = 0; i < inventory.count; i++) {
        bigger[i] = inventory.items[i];
    }
    delete[] inventory.items;
    inventory.items = bigger;
    inventory.capacity = newCapacity;
}

int findStack(const Inventory& inventory, ItemType type)
{
    for (int i = 0; i < inventory.count; i++) {
        if (inventory.items[i].type == type) {
            return i;
        }
    }
    return -1;
}

void removeAt(Inventory& inventory, int index)
{
    for (int i = index; i < inventory.count - 1; i++) {
        inventory.items[i] = inventory.items[i + 1];
    }
    inventory.count--;
}

}

Inventory createInventory()
{
    Inventory inventory;
    inventory.capacity = START_CAPACITY;
    inventory.count = 0;
    inventory.items = new Item[inventory.capacity];
    return inventory;
}

void destroyInventory(Inventory& inventory)
{
    delete[] inventory.items;
    inventory.items = nullptr;
    inventory.count = 0;
    inventory.capacity = 0;
}

void addItem(Inventory& inventory, ItemType type, int artifactId, int quantity)
{
    if (type != ItemType::Artifact) {
        int index = findStack(inventory, type);
        if (index >= 0) {
            inventory.items[index].quantity += quantity;
            return;
        }
    }

    if (inventory.count == inventory.capacity) {
        growInventory(inventory);
    }

    Item item;
    item.type = type;
    item.artifactId = artifactId;
    item.quantity = quantity;
    inventory.items[inventory.count++] = item;
}

bool removeItem(Inventory& inventory, ItemType type, int quantity)
{
    int index = findStack(inventory, type);
    if (index < 0 || inventory.items[index].quantity < quantity) {
        return false;
    }
    inventory.items[index].quantity -= quantity;
    if (inventory.items[index].quantity == 0) {
        removeAt(inventory, index);
    }
    return true;
}

int countOf(const Inventory& inventory, ItemType type)
{
    int index = findStack(inventory, type);
    if (index < 0) {
        return 0;
    }
    return inventory.items[index].quantity;
}

bool hasArtifact(const Inventory& inventory, int artifactId)
{
    for (int i = 0; i < inventory.count; i++) {
        if (inventory.items[i].type == ItemType::Artifact
            && inventory.items[i].artifactId == artifactId) {
            return true;
        }
    }
    return false;
}

int artifactTotal(const Inventory& inventory)
{
    int total = 0;
    for (int i = 0; i < inventory.count; i++) {
        if (inventory.items[i].type == ItemType::Artifact) {
            total++;
        }
    }
    return total;
}
