// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"

#define ODYSSEY_VIRTUALTEXTURE_DATA_MAXLAYERS 8u

class FOdysseyVirtualRenderTargetData
{
public:
    FOdysseyVirtualRenderTargetData()
        : NumLayers(0u)
        , NumMips(0u)
        , Width(0u)
        , Height(0u)
        , WidthInBlocks(0u)
        , HeightInBlocks(0u)
        , TileSize(0u)
        , TileBorderSize(0u)
    {
        FMemory::Memzero(LayerTypes);
        FMemory::Memzero(LayerFallbackColors);
    }

public:
    inline bool IsInitialized() const { return TileSize != 0u; }

    /*uint32 ComputeMipWidth(uint32 InMip) const;
    uint32 ComputeMipHeight(uint32 InMip) const;*/

    //MipMap are smaller versions of the texture to be used when we don't need the full 1:1 version of the texture
    //Example : when the texture is zoomed out, it appears very small, we only a smaller version of this texture
    inline uint32 GetNumMips() const { return NumMips; }

    //Layers : Imagine having lot of kinds of data in a single texture
    // Like BaseColor, Normals, Metalic, Opacity, etc....
    // The texture can't contain all of that data in a single RGBA Buffer
    // That's were layers come in. You just add one layer per type of data you need :
    // Layer 1 : Base Color
    // Layer 2 : Normals
    // Layer 3 : Metalic
    // Layer 4 : Opacity
    // etc...
    inline uint32 GetNumLayers() const { return NumLayers; }

    inline uint32 GetPhysicalTileSize() const { return TileSize + TileBorderSize * 2u; }
    inline uint32 GetWidthInTiles() const { return FMath::DivideAndRoundUp(Width, TileSize); }
    inline uint32 GetHeightInTiles() const { return FMath::DivideAndRoundUp(Height, TileSize); }

    /** Returns true if the requested Tile exists in the TileManager */
    bool TileExists(uint32 InMip, const FIntPoint& InTilePosition) const;

    /** Returns true if the requested Tile is empty or does not exist */
    bool IsTileEmpty(uint32 InMip, const FIntPoint& InTilePosition) const;

    /** Return the byte offset of a tile within a chunk. Returns ~0u if the tile doesn't exist. */
    //uint32 GetTileOffset(uint32 vLevel, uint32 vAddress, uint32 LayerIndex) const;

public:
    uint32 NumLayers;
    uint32 NumMips;
    uint32 Width; // Width of the texture in pixels. Note the physical width may be larger due to tiling
    uint32 Height; // Height of the texture in pixels. Note the physical height may be larger due to tiling
    uint32 WidthInBlocks; // Number of UDIM blocks that make up the texture, used to compute UV scaling factor
    uint32 HeightInBlocks; //UDIM Blocks are a tiling system above the virtual texture tiles
    uint32 TileSize;  // Tile size excluding borders
    uint32 TileBorderSize; // A BorderSize pixel border will be added around all tiles

    /**
     * The pixel format output of the data for each layer. The actual data
     * may still be compressed but will decompress to this pixel format (e.g. zipped DXT5 data).
     */
    TEnumAsByte<EPixelFormat> LayerTypes[ODYSSEY_VIRTUALTEXTURE_DATA_MAXLAYERS];

    /**
     * The fallback color to use for each layer. This color is used whenever we need to sample
     * the VT but have not yet streamed the root pages.
     */
    FLinearColor LayerFallbackColors[ODYSSEY_VIRTUALTEXTURE_DATA_MAXLAYERS];
    struct FMip
    {
        //Maps the tile position to the Id of the tile in the TileManager
        TMap<FIntPoint, FGuid> TileKeyToIndex;
    };

    //One entry per Mip
    TArray<FMip> Mips;
};

#undef ODYSSEY_VIRTUALTEXTURE_DATA_MAXLAYERS
