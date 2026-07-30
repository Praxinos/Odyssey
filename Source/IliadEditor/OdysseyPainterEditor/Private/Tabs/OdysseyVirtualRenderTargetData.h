// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"

#define VIRTUALTEXTURE_DATA_MAXLAYERS 8u

/**
 * Compact structure to find tile offsets within a sparse chunk.
 * The tiles are stored in Morton order inside the chunks. But for UDIM textures some areas of a mip level may be empty and so some tiles don't exist.
 * We could store one chunk offset per tile, but that uses a large amount of memory.
 * So we need a fast, but memory efficient way to get from tile index to chunk location.
 * This structure splits the space into contiguous blocks of empty or non-empty tiles.
 * If a block is non-empty it has an offset into the chunk, and if it is empty it has a special "empty" offset.
 * Within a non-empty block, all tiles exist and are contiguous.
 * Because the tiles are in Morton order and UDIMs are aligned on power of 2 boundaries the sequences are not usually very fragmented, so textures can be described with a low number of blocks.
 * Lookup of tile index is done at the cost of a binary search of Morton address through the stored block addresses.
 */
struct FOdysseyVirtualRenderTargetTileOffsetData
{
#if WITH_EDITOR
    /* Call at start of building the data before any calls to AddTile(). The entire area is initialized as empty. */
    void Init(uint32 InWidth, uint32 InHeight);
    /* Call to mark a tile as non-empty. */
    void AddTile(uint32 InAddress);
    /* Call after all calls to AddTile() to build the final blocks and offsets. */
    void Finalize();
#endif // WITH_EDITOR

    /** Call at runtime to get the final tile offset in the chunk. */
    uint32 GetTileOffset(uint32 InAddress) const;

    /** Serialization helper. */
    friend FArchive& operator<<(FArchive& Ar, FOdysseyVirtualRenderTargetTileOffsetData& TileOffsetData)
    {
        Ar << TileOffsetData.Width;
        Ar << TileOffsetData.Height;
        Ar << TileOffsetData.MaxAddress;
        Ar << TileOffsetData.Addresses;
        Ar << TileOffsetData.Offsets;
        return Ar;
    }

    uint32 Width = 0;
    uint32 Height = 0;
    /** Upper bound Morton address for managed area. */
    uint32 MaxAddress = 0;
    /** Sorted list of contiguous tile block addresses. */
    TArray<uint32> Addresses;
    /** Offset for each block in Addresses. An empty block is marked with ~0u. */
    TArray<uint32> Offsets;

#if WITH_EDITOR
    /** Tile state scratch buffer. Created in Init() and destroyed in Finalize(). */
    TBitArray<> TileStates;
#endif
};

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

    /** Returns false if the address isn't inside the texture bounds. */
    bool IsValidAddress(uint32 vLevel, uint32 vAddress);

    /** Return the byte offset of a tile within a chunk. Returns ~0u if the tile doesn't exist. */
    uint32 GetTileOffset(uint32 vLevel, uint32 vAddress, uint32 LayerIndex) const;

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
    TEnumAsByte<EPixelFormat> LayerTypes[VIRTUALTEXTURE_DATA_MAXLAYERS];

    /**
     * The fallback color to use for each layer. This color is used whenever we need to sample
     * the VT but have not yet streamed the root pages.
     */
    FLinearColor LayerFallbackColors[VIRTUALTEXTURE_DATA_MAXLAYERS];

    /**
     * Per layer tile data offset in bytes.
     * This is empty if compression (zlib etc) is used, since in that case offsets will vary per tile.
     * Each value is the sum of the packed tile data sizes for the current and the preceding layers.
     */
    TArray<uint32> TileDataOffsetPerLayer;

    /** Base offset in chunk for the mip. */
    TArray<uint32> BaseOffsetPerMip;

    /** Holds the tile to chunk offset lookup data. One array entry per mip level. */
    TArray<FOdysseyVirtualRenderTargetTileOffsetData> TileOffsetData;
};

#undef VIRTUALTEXTURE_DATA_MAXLAYERS
