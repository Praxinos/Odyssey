// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         FixedAllocArena.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for FixedAllocArena.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Units.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FFixedAllocArena
/// @brief      The FFixedAllocArena class is a class that provides a single
///             configurable Arena buffer that manages allocations of fixed
///             size. It is meant to be used for tiles or any objects of fixed
///             size.
/// @details    FFixedAllocArena allocs fixed allocations inside its block, and
///             returns "clients" to the allocation, that is, a pointer to an
///             allocation.
///             Using such clients allows us to regularly defragment the Arena,
///             or multiple Arenas if used in a pool, without losing the data
///             if moved to another memory sector.
///
///             The Arena block allocates extra bytes to store some information
///             about the memory status and allocation status, to ensure a fixed
///             number of fixed allocations cells are available.
///
///             The layout of the buffer is like so:
///             - [         chunk        ]
///             - [   cell   ][   cell   ]
///             - [meta][data][meta][data]
///             - [data] contains the real data used by the allocation or tile.
///             - [meta] stores meta information about the allocation such as [client]:
///                 - [client] is a pointer to the allocation, that allows tracking a moving alloc in case of a defrag
///
///             FFixedAllocArena has public methods to estimate the local
///             fragmentation of the memory. To be precise, there is no issue
///             with fragmentation within a single Arena, but it is still possible
///             to measure the sparsity of the allocations. Fragmentation
///             becomes a problem when many arenas are sparse, hence wasting a
///             lot of space.
///
///             A bit of Terminology:
///             - byte_t,     a size in bytes
///             - tByte,      a byte value ( uint8 )
///             - tAlloc,     an allocation ( uint8* )
///             - Arena,      the whole system
///             - block,      the underlying page buffer
///             - metaBase,   the meta info before an alloc
///             - client,     a pointer to an alloc that can change position
///             - cell,       a fixed element in the area that can host an alloc
///             - chunk,      a collection of cells, either free or used
class ULIS_API FFixedAllocArena {
private:
    // Friends
    friend class FFixedAllocMemoryPool; ///< Friend FFixedAllocMemoryPool as it drives the Arena and might need to acces private methods.

private:
    /////////////////////////////////////////////////////
    /// @class      FIterator
    /// @brief      The FFixedAllocArena::FIterator class is a utility helper
    ///             class useful to private parts of FFixedAllocArena only,
    ///             in order to avoid error-prone redundancies in pointer arithmetics.
    class FIterator
    {
    public:
        // Ctors
        explicit
        FIterator(
              tMetaBase iMetaBase
            , uint64 iCellSize
        );

        explicit
        FIterator(
              tClient iClient
            , uint64 iCellSize
        );

        FIterator& operator=( const FIterator& iOther );

    public:
        // Static makers
        static FIterator MakeNull();

    public:
        // Public Methods
        FIterator& operator++();
        FIterator& operator--();
        const FIterator& operator++() const;
        const FIterator& operator--() const;
        bool operator==( const FIterator& iOther ) const;
        bool operator!=( const FIterator& iOther ) const;
        tClient Client();
        const tClient Client() const;
        tMetaBase MetaBase();
        const tMetaBase MetaBase() const;
        tAlloc Allocation();
        const tAlloc Allocation() const;
        bool IsFree() const;
        bool IsUsed() const;
        bool IsValid() const;
        void CleanupMetaBase();
        void FreeClient();
        void ResyncClient();
        tClient AllocClient();
        uint64 CellSize() const;

    private:
        /*!
            This mMetaBase member of type tMetaBase ( uint8* ) will point to parts of the buffer that is passed as an argument
            to the constructor of the FIterator instance. Although the type is uint8_t*, it will remaine valid as
            a way to iterate through any buffer initially passed as a void* type. The uint8_t* type allows to use
            pointer arithmetics directly on it without additional reinterpret casts, and uint8_t* increments in steps
            of one byte.
        */
        mutable tMetaBase mMetaBase; ///< Pointer in arena block.
        uint64 mCellSize; ///< Arena cell size.
    };

public:
    // FFixedAllocArena *-tors
    /*!
        Destructor, destroy the arena.
        Cleanup the underlying arena block.
        Make sure all allocations are free before destroying an arena, or it will trigger an assert in debug builds.
        You can use UnsafeFreeAll() before destruction to ensure it doesn't crash, but only if you know what you're doing.
        It is possible to do so before program termination, but if clients don't expect this, they will all be deleted
        without notification and induce a corrupted state within the program.
    */
    ~FFixedAllocArena();

    /*!
        Constructor from arena size and alloc size.
        Make sure to chose an arena size that is greater than the alloc size and that can fit an expected number of
        fixed cells allocations.
        It is best to chose an alloc size that is a divisor of arena size, the behaviour is undefined otherwise.
    */
    explicit
    FFixedAllocArena(
          byte_t iArenaSize
        , byte_t iAllocSize
    );

    /*!
        Constructor from alloc size and number of expected cells or allocs.
        An arena size will be coomputed so that it can fit an expected number of
        fixed cells allocations.
    */
    explicit
    FFixedAllocArena(
          byte_t iAllocSize
        , uint64 iNumCells
    );

    /*! Explicitely deleted copy constructor */
    FFixedAllocArena( const FFixedAllocArena& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FFixedAllocArena& operator=( const FFixedAllocArena& ) = delete;



public:
    // Public Check API
    /*! Checks wether the arena is full or not. */
    bool IsFull() const;

    /*! Checks wether the arena is empty or not. */
    bool IsEmpty() const;



    // Size API
    /*! Obtain the arena size in bytes */
    byte_t ArenaSize() const;

    /*! Obtain the alloc size in bytes */
    byte_t AllocSize() const;

    /*! Obtain the number of total possible cells in the arena. */
    uint64 NumCells() const;

    /*! Obtain the number of currently free cells in the arena. */
    uint64 NumFreeCells() const;

    /*! Obtain the number of currently used cells in the arena. */
    uint64 NumUsedCells() const;



    // Memory API
    /*!
        Obtain an client to an allocation within this arena.
        If full or a failure occurs, returns nullptr.
    */
    tClient Malloc();

    /*!
        Free an allocation and its associated client.
        It doesn't need to be resident in this arena.
    */
    static void Free( tClient iClient );

    /*!
        Free all resident allocations in this arena.
        Clients are deleted and not notified about their status.
        This is unsafe and dangerous, unless you're done with all clients.
    */
    void UnsafeFreeAll();



    // Occupation API
    /*!
        Determine the local occupation rate of the the allocations within the arena.
        From 0 to 1.
        0: empty
        1: full
        Local fragmentation within a single arena is irrelevant, but the ability to determine the Occupation of fixed allocations
        within an arena matters when used in a pool, with many arenas. A high global average occupation rate indicates the allocs
        are efficiently packed, whereas a low occupation rate indicates it's possiblle to concatenate arenas together and hence
        save space in a pool.
    */
    ufloat OccupationRate() const;



    // Debug API
    /*! Get a textual representation of the arena for debug purposes. */
    void DebugPrint() const;


private:
    // Private Size API
    uint8* LowBlockAdress();
    uint8* HighBlockAdress();
    const uint8* LowBlockAdress() const;
    const uint8* HighBlockAdress() const;
    uint64 BlockSize() const;
    uint64 CellSize() const;
    bool IsResident( tMetaBase iMetaBase );

    // Private Memory API
    static void MoveAlloc( FIterator& iFrom, FIterator& iTo );
    FIterator FindFirst( bool iUsed, const FIterator& iFrom = FIterator::MakeNull() );
    void Initialize();

    // Iterator API
    FIterator Begin();
    FIterator End();
    const FIterator Begin() const;
    const FIterator End() const;

private:
    // Private Data Members
    const uint64 mArenaSize; ///< Arena Size in bytes, without extra meta pad for cell
    const uint64 mAllocSize; ///< Allocation Size in bytes, without extra meta pad for cell
    tByte* const mBlock; ///< Underlying arena storage buffer with allocation data and meta infos [meta][data] ...
};

ULIS_NAMESPACE_END

