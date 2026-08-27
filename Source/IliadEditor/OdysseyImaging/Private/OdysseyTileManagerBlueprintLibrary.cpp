// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTileManagerBlueprintLibrary.h"

#include "OdysseyTileManager.h"

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesUncompressed()
{
    return FOdysseyTileManager::Get().GetNumTilesUncompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeUncompressed()
{
    return FOdysseyTileManager::Get().GetSizeUncompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesCompressed()
{
    return FOdysseyTileManager::Get().GetNumTilesCompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeCompressed()
{
    return FOdysseyTileManager::Get().GetSizeCompressed();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerNumTilesOnDisk()
{
    return FOdysseyTileManager::Get().GetNumTilesOnDisk();
}

int64
UOdysseyTileManagerBlueprintLibrary::GetTileManagerSizeOnDisk()
{
    return FOdysseyTileManager::Get().GetSizeOnDisk();
}

void
UOdysseyTileManagerBlueprintLibrary::EvictAllTiles()
{
    FOdysseyTileManager::Get().EvictAllTiles();
}
