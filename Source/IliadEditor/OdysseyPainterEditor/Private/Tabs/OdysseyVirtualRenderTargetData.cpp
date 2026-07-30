// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTargetData.h"


#if WITH_EDITOR

void FOdysseyVirtualRenderTargetTileOffsetData::Init(uint32 InWidth, uint32 InHeight)
{
    Width = FMath::Max(InWidth, 1u);
    Height = FMath::Max(InHeight, 1u);

    const uint32 SizePadded = FMath::RoundUpToPowerOfTwo(FMath::Max(Width, Height));
    MaxAddress = SizePadded * SizePadded;

    Addresses.Empty();
    Offsets.Empty();
    TileStates.Empty(MaxAddress);
    TileStates.Add(false, MaxAddress);
}

void FOdysseyVirtualRenderTargetTileOffsetData::AddTile(uint32 InAddress)
{
    TileStates[InAddress] = true;
}

void FOdysseyVirtualRenderTargetTileOffsetData::Finalize()
{
    uint32 Offset = 0;
    uint32 StartAddress = 0;
    bool bCurrentState = TileStates[0];

    Addresses.Add(StartAddress);
    Offsets.Add(bCurrentState ? Offset : ~0u);

    for (uint32 Address = 1; Address < MaxAddress; ++Address)
    {
        const bool bState = TileStates[Address];
        if (bState != bCurrentState)
        {
            Offset += bCurrentState ? Address - StartAddress : 0;

            StartAddress = Address;
            bCurrentState = bState;

            Addresses.Add(StartAddress);
            Offsets.Add(bCurrentState ? Offset : ~0u);
        }
    }

    TileStates.Empty();
}

#endif // WITH_EDITOR

uint32 FOdysseyVirtualRenderTargetTileOffsetData::GetTileOffset(uint32 InAddress) const
{
    const int32 BlockIndex = Algo::UpperBound(Addresses, InAddress) - 1;
    const uint32 BaseOffset = Offsets[BlockIndex];
    if (BaseOffset == ~0u)
    {
        // Address is in empty space.
        return ~0u;
    }
    const uint32 BaseAddress = Addresses[BlockIndex];
    const uint32 LocalOffset = InAddress - BaseAddress;
    return BaseOffset + LocalOffset;
}

bool FOdysseyVirtualRenderTargetData::IsValidAddress(uint32 vLevel, uint32 vAddress)
{
    bool bIsValid = false;

    if (TileOffsetData.IsValidIndex(vLevel))
    {
        const uint32 X = FMath::ReverseMortonCode2(vAddress);
        const uint32 Y = FMath::ReverseMortonCode2(vAddress >> 1);
        bIsValid = X < TileOffsetData[vLevel].Width && Y < TileOffsetData[vLevel].Height;
    }

    return bIsValid;
}


uint32 FOdysseyVirtualRenderTargetData::GetTileOffset(uint32 vLevel, uint32 vAddress, uint32 LayerIndex) const
{
    uint32 Offset = ~0u;

    if (BaseOffsetPerMip.IsValidIndex(vLevel) && TileOffsetData.IsValidIndex(vLevel))
    {
        // If the tile offset is ~0u there is no data present so we return ~0u to indicate that.
        const uint32 BaseOffset = BaseOffsetPerMip[vLevel];
        const uint32 TileOffset = TileOffsetData[vLevel].GetTileOffset(vAddress);
        if (BaseOffset != ~0u && TileOffset != ~0u)
        {
            const uint32 TileDataSize = TileDataOffsetPerLayer.Last();
            const uint32 LayerDataOffset = LayerIndex == 0 ? 0 : TileDataOffsetPerLayer[LayerIndex - 1];

            int64 Offset64 = BaseOffset + (int64) TileOffset * TileDataSize + LayerDataOffset;
            Offset = IntCastChecked<uint32>( Offset64 );
        }
    }

    return Offset;
}
