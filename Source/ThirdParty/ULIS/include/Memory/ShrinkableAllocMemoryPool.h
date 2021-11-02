// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ShrinkableAllocMemoryPool.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for FShrinkableAllocMemoryPool.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/ShrinkableAllocArena.h"
#include "Memory/MemoryPoolPolicy.h"
#include <list>
#include <functional>

ULIS_NAMESPACE_BEGIN
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FShrinkableAllocMemoryPool
/// @brief      The FShrinkableAllocMemoryPool class is a class that provides a
///             configurable pool of shrinkable alloc arenas buffer to manages
///             allocations with known maximum size data efficiently and optimize memory
///             consumption by evaluating sparsity, occupation and fragmentation
///             of the various arena pages and concatenate them if neccessary.
///             It is meant to be used for RLE Compressed tiles or any objects of known maximal
///             size, potentialy shrinkable, and can be driven asynchronously by a larger system if
///             necessary.
/// @details    FShrinkableAllocMemoryPool allows to obtain shrinkable allocations that
///             are resident of one arena page underlying block. It returns  "clients"
///             of the allocation, that is, a pointer to an allocation.
///             The API is mostly similar to FShrinkableAllocArena, but the pool aggregates
///             them in a list and manages fragmentation and higher level systems and
///             algorithms to drive them efficiently.
class ULIS_API FShrinkableAllocMemoryPool {
public:
    /*!
        Destructor, destroy the arenas pages.
        Cleanup the underlying arenas block.
        Make sure all allocations are free before destroying a pool, or it will trigger an assert in debug builds.
        You can use UnsafeFreeAll() before destruction to ensure it doesn't crash, but only if you know what you're doing.
        It is possible to do so before program termination, but if clients don't expect this, they will all be deleted
        without notification and induce a corrupted state within the program.
    */
    ~FShrinkableAllocMemoryPool();

    /*!
        Constructor from arena size and max alloc size.
        Make sure to chose an arena size that is greater than the alloc size and that can fit an expected number of
        fixed cells allocations.
    */
    explicit
    FShrinkableAllocMemoryPool(
          byte_t iArenaSize
        , byte_t iMaxAllocSize
        , byte_t iTargetMemoryUsage = 1
        , ufloat iDefragThreshold = 1/3.f
        , const FMemoryPoolPolicy& iPolicy = FMemoryPoolPolicy::PermissiveGrowth
    );

    /*!
        Constructor from alloc size and number of expected cells or allocs.
        An arena size will be computed so that it can fit an expected number of
        max size cells allocations.
    */
    explicit
    FShrinkableAllocMemoryPool(
          byte_t iMaxAllocSize
        , uint64 iNumCellPerArena
        , byte_t iTargetMemoryUsage = 1
        , ufloat iDefragThreshold = 1/3.f
        , const FMemoryPoolPolicy& iPolicy = FMemoryPoolPolicy::PermissiveGrowth
    );

    /*! Explicitely deleted copy constructor */
    FShrinkableAllocMemoryPool( const FShrinkableAllocMemoryPool& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FShrinkableAllocMemoryPool& operator=( const FShrinkableAllocMemoryPool& ) = delete;

public:
    // Size API
    /*! Obtain the arena size in bytes */
    byte_t ArenaSize() const;

    /*! Obtain the alloc size in bytes */
    byte_t MaxAllocSize() const;



    // Memory API
    /*! Obtain the total theoretical memory for this pool as bytes, given the current number or arenas. */
    byte_t TotalMemory() const;

    /*! Obtain current free memory as bytes. */
    byte_t FreeMemory() const;

    /*! Obtain current used memory as bytes. */
    byte_t UsedMemory() const;

    /*! Obtain the target memory usage as bytes. */
    byte_t TargetMemoryUsage() const;

    /*! Set the target memory usage as bytes. */
    void SetTargetMemoryUsage( byte_t iValue );

    /*! Get modifiable Memory Policy. */
    FMemoryPoolPolicy& MemoryPolicy();

    /*! Get const Memory Policy. */
    const FMemoryPoolPolicy& MemoryPolicy() const;



    // Fragmentation API
    /*! Get the defrag theshold. */
    float DefragThreshold() const;

    /*! Set the defrag theshold. */
    void SetDefragThreshold( ufloat iValue );

    /*! Arbitrary heuristic to estimate fragmentation of the pool. */
    ufloat Fragmentation() const;

    /*! Trigger a defrag only if the threshold is reached. */
    void DefragIfNecessary();

    /*! Force trigger a defrag. */
    void DefragForce();



    // Alloc API
    /*!
        Obtain an client to an allocation within this pool.
        If full or a failure occurs, returns nullptr.
    */
    tClient Malloc( byte_t iAllocSize = ULIS_UINT32_MAX );

    /*!
        Free an allocation and its associated client.
        It doesn't need to be resident in this pool.
    */
    static void Free( tClient iClient );

    /*!
        Free all resident allocations in this pool.
        Clients are deleted and not notified about their status.
        This is unsafe and dangerous, unless you're done with all clients.
    */
    void UnsafeFreeAll();

    /*!
        Alloc empty arena page if target memory isn't reached.
    */
    bool AllocOneArenaIfNecessary();

    /*!
        Delete empty arena page if target memory is reached.
    */
    bool FreeOneArenaIfNecessary();

    /*!
        Alloc N empty arenas until memory target is reached.
        Return the number of allocated arenas.
    */
    uint64 AllocArenasToReachTargetMemory();

    /*!
        Free all arenas that are empty.
    */
    uint64 FreeEmptyArenasToReachTargetMemory();

    /*!
        Free all arenas that are empty.
    */
    uint64 FreeAllEmptyArenas();

    /*!
        Free arenas according to predicate.
    */
    uint64 FreeEmptyArenasAccordingToPredicate( std::function< bool() > iPredicate );



    // Debug API
    /*! Get a textual representation of the arena for debug purposes. */
    void DebugPrint( int iType = 0, int iCol = 100 ) const;

private:
    const uint64 mArenaSize; ///< Arena Size in bytes, with extra meta pad for cell
    const uint64 mMaxAllocSize; ///< Max allocation Size in bytes, without extra meta pad for cell
    uint64 mTargetMemoryUsage; ///< approximate target memory to reach with arena pages, actual behaviour depends on policy.
    ufloat mDefragThreshold; ///< A threshold that, if reached, allows to trigger a defrag.
    std::list< FShrinkableAllocArena* > mArenaPool; ///< The arenas pages, in a list.
    FMemoryPoolPolicy mPolicy; ///< The memory policy
};
#pragma warning(pop)
ULIS_NAMESPACE_END

