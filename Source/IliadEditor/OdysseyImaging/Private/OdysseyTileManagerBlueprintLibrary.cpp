// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManagerBlueprintLibrary.h"

#include "OdysseyTileManager.h"

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesUncompressed()
{
    return FOdysseyTileManager::Get().GetStats().GetNumTilesUncompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeUncompressed()
{
    return FOdysseyTileManager::Get().GetStats().GetSizeUncompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesCompressed()
{
    return FOdysseyTileManager::Get().GetStats().GetNumTilesCompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeCompressed()
{
    return FOdysseyTileManager::Get().GetStats().GetSizeCompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesOnDisk()
{
    return FOdysseyTileManager::Get().GetStats().GetNumTilesOnDisk();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeOnDisk()
{
    return FOdysseyTileManager::Get().GetStats().GetSizeOnDisk();
}

void
UOdysseyTileManagerBlueprintLibrary::EvictAllTiles()
{
    FOdysseyTileManager::Get().EvictAllTiles();
}
