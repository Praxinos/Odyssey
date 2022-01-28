// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         UncompressedMemoryPool.h
* @author       Clement Berthaud
* @brief        This file provides declaration for the MemoryDriver class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Units.h"
#include "Sparse/UncompressedMemoryPool.h"

#include <atomic>
#include <list>
#include <forward_list>
#include <mutex>
#include <thread>
#include <unordered_map>

ULIS_NAMESPACE_BEGIN
class FUncompressedMemoryPool;
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FMemoryDriver
/// @brief      The is a subcomponent of FTilePool that manages busy tile
///             memory.
class ULIS_API FMemoryDriver
{
public:
    // Construction / Destruction
    /*! Destructor. */
    ~FMemoryDriver();

    /*! Constructor. */
    FMemoryDriver(
          const uint8* iBackground
        , uint32 iBackgroundHash
        , byte_t iTileSize
        , uint64 iNumCellPerArena
        , byte_t iTargetMemoryUsage
        , ufloat iDefragThreshold = 1/3.f
        , double iUncompressedMemoryPoolRelaxTime = 0.0
        , uint32 iDeallocBatchSize = 1
        , uint32 iAllocBatchSize = 1
        , double iWorkerRelaxTime_ms = 0.0
        , uint32 iDirtyHashedBatchSize = 0
        , uint32 iCorrectlyHashedBatchSize = 0
    );

    /*! Explicitely deleted copy constructor */
    FMemoryDriver( const FMemoryDriver& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FMemoryDriver& operator=( const FMemoryDriver& ) = delete;

public:
    // Core API
    void PurgeAllNow();
    void SanitizeNow();
    FTile* QueryOne();
    FTile* RedundantHashMerge( FTile* iTile );
    void SetBackground( const uint8* iBackground, uint32 iBackgroundHash );

private:
    // Private Workers API
    // Worker sub-processes
    void SanitizeDirtyHashedBatch();
    void SanitizeCorrectlyHashedBatch();
    // Main worker
    void WorkProcess();

private:
    // Private Data Members
    FUncompressedMemoryPool mUncompressedMemoryPool;
    uint32 mBackgroundHash;
    std::list< FTile* > mDirtyHashedTiles;
    std::unordered_map< uint32, FTile* > mCorrectlyHashedTiles;
    std::mutex mMutexCorrectlyHashedTilesLock;
    std::mutex mMutexDirtyHashedTilesLock;
    const uint64 mBytesPerTile;
    std::atomic< bool > bStopWorker;
    std::thread* const mWorker;
    double mWorkerRelaxTime_ms;
    uint32 mCorrectlyHashedBatchSize;
    uint32 mDirtyHashedBatchSize;
};
#pragma warning(pop)

ULIS_NAMESPACE_END

