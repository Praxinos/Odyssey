// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         TilePool.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TilePool class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"
#include "Data/Sparse/TiledBlock.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// TTilePool
class ULIS3_API ITilePool
{
public:
    // Construction / Destruction
    virtual ~ITilePool() {}

public:
    // Static Maker
    static ITilePool* XCreateTilePool( tFormat          iFormat
                                     , FColorProfile*   iProfile
                                     , eMicro           iMicro
                                     , eMacro           iMacro );
    static void XDeleteTilePool( ITilePool* iTilePool );

public:
    // Public API
    virtual const FVec2I&                               TileSize()                              const       = 0;
    virtual uint32                                      EmptyCRC32Hash()                        const       = 0;
    virtual const FBlock*                               EmptyTile()                             const       = 0;
    virtual tFormat                                     TileFormat()                            const       = 0;
    virtual const FFormatInfo&                          TileFormatInfo()                        const       = 0;
    virtual const FColorProfile*                        TileColorProfile()                      const       = 0;
    virtual uint64                                      CurrentRAMUsage()                       const       = 0;
    virtual uint64                                      CurrentSwapUsage()                      const       = 0;
    virtual uint64                                      RAMUsageCapTarget()                     const       = 0;
    virtual uint64                                      SWAPUsageCapTarget()                    const       = 0;
    virtual uint64                                      CurrentTotalMemoryUsage()               const       = 0;
    virtual uint64                                      NumTilesScheduledForClear()             const       = 0;
    virtual uint64                                      NumFreshTilesAvailableForQuery()        const       = 0;
    virtual uint64                                      NumDirtyHashedTilesCurrentlyInUse()                 = 0;
    virtual uint64                                      NumCorrectlyHashedTilesCurrentlyInUse()             = 0;
    virtual uint64                                      NumRegisteredTiledBlocks()                          = 0;
    virtual ITiledBlock*                                CreateNewTiledBlock()                               = 0;
    virtual void                                        RequestTiledBlockDeletion( ITiledBlock* iBlock )    = 0;
    virtual void                                        SetRAMUsageCapTarget( uint64 iValue )               = 0;
    virtual void                                        SetSWAPUsageCapTarget( uint64 iValue )              = 0;

public:
    // Core API
    virtual void                                        PurgeAllNow() = 0;
    virtual FTileElement*                               XQueryFreshTile() = 0;
    virtual FTileElement*                               PerformRedundantHashMergeReturnCorrect( FTileElement* iElem ) = 0;
    virtual FTileElement*                               XPerformDataCopyForImminentMutableChangeIfNeeded( FTileElement* iElem ) = 0;
};

ULIS3_NAMESPACE_END
