// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

/**
 * The custom version of the serialization of UOdysseyTiledImage.
 * Every package holding tiled images saves it; packages saved before the tiled image storage do not.
 */
struct ODYSSEYIMAGING_API FOdysseyTiledImageObjectVersion
{
    enum Type
    {
        // Before any version changes were made
        SerializeTiles,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    /** Identifies this custom version in package files. */
    const static FGuid GUID;

private:
    FOdysseyTiledImageObjectVersion() {}
};
