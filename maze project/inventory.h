#pragma once

#include "types.h"

Inventory createInventory();
void destroyInventory(Inventory& inventory);
void addItem(Inventory& inventory, ItemType type, int artifactId, int quantity);
bool removeItem(Inventory& inventory, ItemType type, int quantity);
int countOf(const Inventory& inventory, ItemType type);
bool hasArtifact(const Inventory& inventory, int artifactId);
int artifactTotal(const Inventory& inventory);
