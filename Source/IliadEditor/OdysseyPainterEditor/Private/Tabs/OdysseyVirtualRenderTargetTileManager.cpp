// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetTileManager.h"

//static
FOdysseyVirtualRenderTargetTileManager&
FOdysseyVirtualRenderTargetTileManager::Get()
{
    static FOdysseyVirtualRenderTargetTileManager self;
    return self;
}

FGuid
FOdysseyVirtualRenderTargetTileManager::AddTile(uint32 InWidth, uint32 InHeight, uint32 InBytesPerPixel)
{
    FTileDescriptor Desc;

    Desc.Width = InWidth;
    Desc.Height = InHeight;
    Desc.BytesPerPixel = InBytesPerPixel;

    FGuid Id = FGuid::NewGuid();

    TileDescriptors.Add(Id, Desc);

    return Id;
}

bool
FOdysseyVirtualRenderTargetTileManager::GetTileDescriptor(const FGuid& InTileIndex, FTileDescriptor& OutTileDesc) const
{
    const FTileDescriptor* desc = TileDescriptors.Find(InTileIndex);
    if (desc)
        OutTileDesc = *desc;
    return !!desc;
}
