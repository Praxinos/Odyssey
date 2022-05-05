// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TilePool.h
* @author       Clement Berthaud
* @brief        This file provides declaration for the TilePool class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Sparse/TiledBlock.h"
#include "Image/Format.h"
#include "Image/ColorSpace.h"
#include "Sparse/MemoryDriver.h"
#include "Memory/Units.h"

#include <list>

ULIS_NAMESPACE_BEGIN
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FTilePool
/// @brief      The FTilePool class stores redundant tiles and manages tiled blocks.
class ULIS_API FTilePool
    : public IHasFormat
    , public IHasColorSpace
{
private:
    friend class FTiledBlock;
    friend class FLQTree;

public:
    // Construction / Destruction
    /*! Destructor. */
    ~FTilePool();

    /*! Constructor. */
    FTilePool(
          eFormat iFormat = eFormat::Format_RGBA8
        , FColorSpace* iColorSpace = nullptr
    );

    /*! Explicitely deleted copy constructor */
    FTilePool( const FTilePool& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FTilePool& operator=( const FTilePool& ) = delete;

public:
    // Public API
    FVec2I TileSize() const;
    uint32 EmptyCRC32Hash() const;
    const uint8* EmptyTile() const;
    void PrintDiagnosis();

private:
    void RegisterTiledBlock( FTiledBlock* iBlock );
    void UnregisterTiledBlock( FTiledBlock* iBlock );

public:
    // Core API

private:
    void PurgeAllNow();
    FTile* QueryOne();
    FTile* RedundantHashMerge( FTile* iElem );
    FTile* SplitMutable( FTile* iElem );

private:
    // Private Data Members
    // Registered Blocks
    std::list< FTiledBlock* > mRegisteredTiledBlocks;

    // Empty Tile
    FBlock* const mEmptyTile;
    uint32 mEmptyCRC32Hash;
    const uint64 mBytesPerTile;

    // Drivers
    FMemoryDriver* mMemoryDriver;
};
#pragma warning(pop)

ULIS_NAMESPACE_END

