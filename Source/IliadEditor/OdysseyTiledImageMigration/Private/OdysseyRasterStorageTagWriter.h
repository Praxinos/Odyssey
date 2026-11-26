// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"

class FAssetRegistryTagsContext;

/**
 * Adds Odyssey::TiledImageMigration::RasterStorageTag to every saved asset whose package holds raster layers or cells,
 * whether or not they hold content.
 */
class FOdysseyRasterStorageTagWriter
{
public:
    /** Starts adding the tag to the saved assets. */
    void Start();

    /** Stops adding the tag. */
    void Stop();

private:
    /** Handler of OnGetExtraObjectTagsWithContext: adds the tag to the saved asset of Context. */
    static void AddTag(FAssetRegistryTagsContext Context);

private:
    /** The subscription to the extra tags of saved assets. */
    FDelegateHandle ExtraTagsHandle;
};
