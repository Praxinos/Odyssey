// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FOdysseyVirtualRenderTargetTileManager
{
public:
    static FOdysseyVirtualRenderTargetTileManager& Get();

private:
    FOdysseyVirtualRenderTargetTileManager() = default;

public:
    struct FTileDescriptor
    {
        bool bIsEmpty = true;
        uint32 Width = 0;
        uint32 Height = 0;
        uint32 BytesPerPixel = 0;
    };

public:
    //Adds an empty tile
    FGuid AddTile(uint32 InWidth, uint32 InHeight, uint32 InBytesPerPixel);
    bool GetTileDescriptor(const FGuid& InTileIndex, FTileDescriptor& OutTileDesc) const;

private:
    TMap<FGuid, FTileDescriptor> TileDescriptors;
};
