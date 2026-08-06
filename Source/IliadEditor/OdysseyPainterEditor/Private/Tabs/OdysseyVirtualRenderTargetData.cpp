// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetData.h"

#include "OdysseyVirtualRenderTargetTileManager.h"

/* uint32
FOdysseyVirtualRenderTargetData::ComputeMipWidth(uint32 InMip) const
{
    if (Mips.IsEmpty())
        return 0;

    if (InMip >= Mips.Num())
    {
        uint32 mipWidth = ComputeMipWidth(Mips.Num() - 1);

    }


    //Find an available Mip


}

uint32
FOdysseyVirtualRenderTargetData::ComputeHeight() const
{
    if (Mips.IsEmpty())
        return 0;
}*/

bool
FOdysseyVirtualRenderTargetData::TileExists(uint32 InMip, const FIntPoint& InTilePosition) const
{
    if (InMip >= (uint32)Mips.Num())
        return false;

    return Mips[InMip].TileKeyToIndex.Contains(InTilePosition);
}

bool
FOdysseyVirtualRenderTargetData::IsTileEmpty(uint32 InMip, const FIntPoint& InTilePosition) const
{
    //A non existing tile can be considered as empty
    //Anyway, calling TileExists() seperately is better practice
    if (!TileExists(InMip, InTilePosition))
        return true;

    FGuid TileIndex = Mips[InMip].TileKeyToIndex[InTilePosition];

    FOdysseyVirtualRenderTargetTileManager& TileManager = FOdysseyVirtualRenderTargetTileManager::Get();
    FOdysseyVirtualRenderTargetTileManager::FTileDescriptor TileDesc;

    bool TileFound = TileManager.GetTileDescriptor(TileIndex, TileDesc);
    check(TileFound);
    if (!TileFound)
        return true;

    return TileDesc.bIsEmpty;
}
