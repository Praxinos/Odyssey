// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TiledBlock.h
* @author       Clement Berthaud
* @brief        This file provides declaration for the TiledBlock class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Sparse/LinearQuadtree.h"
#include "Image/Format.h"
#include "Image/ColorSpace.h"
#include <unordered_map>

ULIS_NAMESPACE_BEGIN
class FTilePool;
class FLQTree;
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FTiledBlock
/// @brief      The FTiledBlock class provides a mean of storing very large tiled images,
///             powered by a large number of complex algorithms allowing efficient memory
///             usage and performances, as well as disk streaming facilities, compression
///             systems, redundant data reuse by hashing pixel data, low overall rates in
///             memory fragmentation, infinite or unbounded images in space and time, within
///             reasonable range ( depends on the user available disk space and ram space ),
///             and multithreaded operations.
class ULIS_API FTiledBlock
{
public:
    // Construction / Destruction
    /*! Destructor, cleanup chunks. */
    ~FTiledBlock();

    /*! Constructor, initialize with zero chunks. */
    FTiledBlock( FTilePool& iTilePool );

    /*! Explicitely deleted copy constructor */
    FTiledBlock( const FTiledBlock& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FTiledBlock& operator=( const FTiledBlock& ) = delete;

public:
    // Core API
    /*! Get the number of chunk entries in the map */
    uint64 NumChunks() const;

    /*! Gather all chunks in an array */
    void GatherChunks( TArray< FLQTree* >* oVec );

    /*! check wether the input coordinate is in a valid range */
    static bool IsValidPixelCoordRange( int64 iValue );

    /*! Coordinates */
    FVec2I ChunkCoordinatesFromPixelCoordinates( const FVec2I& iPos ) const;

    /*! Coordinates */
    FVec2I PixelCoordinatesFromChunkCoordinates( const FVec2I& iPos ) const;

    /*! Coordinates */
    uint64 KeyFromChunkCoordinates( const FVec2I& iPos ) const;

    /*! Coordinates */
    uint64 KeyFromPixelCoordinates( const FVec2I& iPos ) const;

    /*! Coordinates */
    FVec2I ChunkCoordinatesFromKey( uint64 iKey ) const;

    /*! Coordinates */
    FVec2I PixelCoordinatesFromKey( uint64 iKey ) const;

public:
    // Block API
    /*! Return the operative geometry */
    const FRectI& OperativeGeometry() const;

    /*! Return the rough root geometry made up from chunks */
    const FRectI& RootGeometry() const;

    /*! Return the rough leaf geometry built from all leafs */
    const FRectI& LeafGeometry() const;

    /*! Process operative geometry */
    void ExtendOperativeGeometryAfterMutableChange( const FRectI& iRect );

    /*! Process operative geometry */
    void SubstractOperativeGeometryAfterMutableChange( const FRectI& iRect );

    /*! Manual recompute cached root geometry */
    void RecomputeRootGeometry();

    /*! Manual recompute cached leaf geometry */
    void RecomputeLeafGeometry();

private:
    // Private API
    /*! Internal Query Chunk for Read / Write, create chunk if not exist */
    FLQTree* QueryChunkRW( const FVec2I& iPos );

    /*! Internal Query Chunk for Read only, return nullptr if not exist */
    const FLQTree* QueryChunkR( const FVec2I& iPos ) const;

public:
    // Tile API
    /*! Query a tile for Read only */
    const uint8* QueryConstTile( const FVec2I& iPos ) const;

    /*! Query a tile for Read Write */
    FTile** QueryMutableTile( const FVec2I& iPos );

    /*! Clear all map */
    void Clear();

    /*! Sanitize map, removing pure empty chunks */
    void SanitizeNow();

private:
    // Private Data Members
    // key uint64
    // 1) 32bits: packed int32 Y
    // 2) 32bits: packed int32 X
    std::unordered_map< uint64, FLQTree* > mChunks;
    FTilePool& mTilePool;
    FRectI mOperativeGeometry;
    FRectI mRootGeometry;
    FRectI mLeafGeometry;

    static constexpr int32 sm_chunk_min_coord = std::numeric_limits< int32 >::min();
    static constexpr int32 sm_chunk_max_coord = std::numeric_limits< int32 >::max();
    static constexpr int64 sm_pixel_min_coord = static_cast< int64 >( sm_chunk_min_coord ) * FLQTree::sm_root_size_as_pixels;
    static constexpr int64 sm_pixel_max_coord = static_cast< int64 >( sm_chunk_max_coord ) * FLQTree::sm_root_size_as_pixels;
};
#pragma warning(pop)

ULIS_NAMESPACE_END

