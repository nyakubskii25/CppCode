#include "artifacts.h"

#include <cstdlib>

#include "inventory.h"

using namespace std;

ArtifactInfo* createCatalog(int& count)
{
    count = ARTIFACT_COUNT;
    ArtifactInfo* catalog = new ArtifactInfo[count];

    catalog[ARTIFACT_LENS] = { ARTIFACT_LENS, "Crystal Lens", "You see 3 cells further in the fog." };
    catalog[ARTIFACT_MAP] = { ARTIFACT_MAP, "Ancient Map", "Press M to see the whole level once per level." };
    catalog[ARTIFACT_COMPASS] = { ARTIFACT_COMPASS, "Golden Compass", "Press C to find the way to the exit." };
    catalog[ARTIFACT_RING] = { ARTIFACT_RING, "Lucky Ring", "Every coin is worth twice as many points." };

    return catalog;
}

void destroyCatalog(ArtifactInfo*& catalog)
{
    delete[] catalog;
    catalog = nullptr;
}

int pickRandomArtifact(const Inventory& inventory)
{
    int missing[ARTIFACT_COUNT];
    int missingCount = 0;
    for (int id = 0; id < ARTIFACT_COUNT; id++) {
        if (!hasArtifact(inventory, id)) {
            missing[missingCount++] = id;
        }
    }
    if (missingCount == 0) {
        return -1;
    }
    return missing[rand() % missingCount];
}

int visionBonus(const Inventory& inventory)
{
    return hasArtifact(inventory, ARTIFACT_LENS) ? LENS_BONUS : 0;
}

int coinMultiplier(const Inventory& inventory)
{
    return hasArtifact(inventory, ARTIFACT_RING) ? 2 : 1;
}
