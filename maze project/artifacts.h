#pragma once

#include "types.h"

ArtifactInfo* createCatalog(int& count);
void destroyCatalog(ArtifactInfo*& catalog);
int pickRandomArtifact(const Inventory& inventory);
int visionBonus(const Inventory& inventory);
int coinMultiplier(const Inventory& inventory);
