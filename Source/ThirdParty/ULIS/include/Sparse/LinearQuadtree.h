// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LinearQuadtree.h
* @author       Clement Berthaud
* @brief        This file provides declaration for the FLQTree class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Math.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Memory/LimitedArray.h"
#include "Sparse/Tile.h"

ULIS_NAMESPACE_BEGIN
class FTilePool;
//template class ULIS_API TLimitedArray< FTile*, 256 >;
/////////////////////////////////////////////////////
/// @class      FLQTree
/// @brief      The FLQTree class provides a quadtree that uses an
///             efficient linear scheme to store data and avoid many pointers
///             indirection, overhead, and memory cost.
/// @details    A linear quadtree is a typical way to optimize a quadtree and
///             guarantee constant time access to leafs and neighbours.
///             It also allows to remove the actual memory cost of a pointer
///             based structure, and access to leafs elements is computed from
///             a simple formula.
///             See: morton codes, space filling curve Z-order curve.
///
///             Deep copy and Shallow copy are explicitely forbidden.
///
///             Leaf entries are simply called attributes associated with
///             a leaf type code, the corresponding attribute value is searched in
///             an attribute array associated with the leaf type.
///             Attribute arrays are TArrays with a reserved size that is a multiple of
///             the maximum depth at wich one or more leaf exist, and cannot exceed
///             the maximum number of max depth leafs.
class ULIS_API FLQTree
{
    typedef FLQTree tSelf;

public:
    /*! Destructor, cleanup bulk and attributes. */
    ~FLQTree();

    /*! Constructor, initialize empty quadtree structure. */
    FLQTree();

    /*! Explicitely deleted copy constructor */
    FLQTree( const tSelf& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    tSelf& operator=( const tSelf& ) = delete;

public:
    // Public API
    /*! Erase tile at pixel coordinates */
    void Erase( uint16 iX, uint16 iY );

    /*! Clear all of this QTree*/
    void Clear();

    /*! Rough leaf geometry in local QTree referential as pixel sizes */
    FRectI LeafGeometry() const;

    /*! Query const client data at pixel coordinates ( Read-only ) */
    const uint8* QueryConst( FTilePool& iPool, uint8 iX, uint8 iY ) const;

    /*! Query one mutable tile element for imminent dirty operation at pixel coordinates */
    FTile** QueryMutable( FTilePool& iPool, uint8 iX, uint8 iY );

    /*! Check if the LQT is empty, might be deleted if so. */
    bool IsEmpty() const;

    /*! Sanitize. */
    void SanitizeNow( FTilePool& iPool );

private:
    // Private Data Members
    // Bulk:
    // constant size 2048o
    // still better than worst-case in pointer-based implementation
    // using direct pointers to avoid using a hash map or attribute array
    // more perf, at the cost of some memory cost
    // the trade off is good enough, i'm more interested in perf than memory here.
    // a 2Kio per quadtree chunk still leaves plenty of room
    // for a 299 008² image,
    // 292² chunks, 85 264 * 2Kio = 170 528Kio = ~166Mio of storage
    FTile* mBulk[256];

public:
    uint8 mNumEntries;
    //static constexpr uint8 sm_num_types = 3; // Empty, Filled, Data, RLE, Disk
    static constexpr uint8 sm_leaf_threshold = 6; // 2^6 = 64
    static constexpr uint8 sm_root_threshold = 4; // 2^4 = 16
    static constexpr uint16 sm_leaf_size_as_pixels = FMath::ContexprPow( 2, sm_leaf_threshold ); // 64
    static constexpr uint16 sm_root_size_as_leafs = FMath::ContexprPow( 2, sm_root_threshold ); // 16
    static constexpr uint32 sm_root_size_as_pixels = sm_leaf_size_as_pixels * sm_root_size_as_leafs; // 1024
    static constexpr uint32 sm_num_leafs = sm_root_size_as_leafs * sm_root_size_as_leafs; // 256
    //static constexpr uint8 sm_type_span = FMath::ConstexprINeededBitsForRange( sm_num_types ); // 2
    //static constexpr uint8 sm_attribute_span = static_cast< uint8 >( FMath::ConstexprINeededBitsForRange( sm_num_leafs ) ); // 8
};

ULIS_NAMESPACE_END

