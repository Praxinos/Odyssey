// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Tile.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Tile class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <atomic>

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// FTileElement
struct ULIS3_API FTileElement {

    ~FTileElement()
    {
        ULIS3_ASSERT( mRefCount.load() == 0, "Bad RefCount on Delete Tile" );
    }

    FTileElement()
        : mBlock( nullptr )
        , mHash( 0 )
        , mRefCount( 0 )
        , mDirty( false )
    {}

    FTileElement( FBlock* iPtr )
        : mBlock( iPtr )
        , mHash( 0 )
        , mRefCount( 0 )
        , mDirty( true )
    {}

    void DecreaseRefCount() { mRefCount--; };
    void IncreaseRefCount() { mRefCount++; };

    FBlock* mBlock;
    uint32  mHash;
    std::atomic< uint32 >   mRefCount;
    std::atomic< bool >     mDirty;
};

ULIS3_NAMESPACE_END

