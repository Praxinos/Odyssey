// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ShrinkableAllocArena.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for ShrinkableAllocArena.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Units.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FShrinkableAllocArena
/// @brief      The FShrinkableAllocArena provides a  configurable Arena buffer that manages
///             allocations with a maximal size, that can shrink over time.
///             It is meant to be used for RLE tiles or any objects with a known maximal size
///             that can shrink. It is perfect for RLE because we know the max size of an
///             incompressible tile, but we don't know the min size until the compression is done.
///             A collection of such shrinkable allocs is subject to a high fragmentation rate.
/// @details    FShrinkableAllocArena allocs initially fixed max size allocations inside its block,
///             and returns "clients" to the allocation, that is, a pointer to an
///             allocation.
///             Using such clients allows us to regularly defragment the Arena,
///             or multiple Arenas if used in a pool, without losing the data
///             if moved to another memory sector.
///
///             The Arena block allocates extra bytes to store some information
///             about the memory status and allocation status, to ensure a fixed
///             number of fixed allocations cells are available.
///
///             The layout of the buffer is defined as follows:
///             - [meta][data]
///             - [data] contains the real data used by the allocation or tile.
///             - [meta] stores meta information about the allocation such as [client][prev][next]:
///                 - [client] is a pointer to the allocation, that allows tracking a moving alloc in case of a defrag
///                 - [prev] is a delta in bytes to the prev metabase
///                 - [next] is a delta in bytes to the next metabase
///             The very first element of the buffer should have a prev delta of zero, it acts as a sentinel.
///             We also need a way to identify the case when we reach the end of the arena, so we add another sentinel at the end
///             with a next meta size of 0. No other allocation can have a next meta size of 0.
///
///             FShrinkableAllocArena has public methods to estimate the local
///             fragmentation of the memory. Fragmentation matters here in an arena with
///             variable size even within a single Arena. The method used to determine
///             fragmentation is arbitrary but serves the purpose of estimating when a
///             defragmentation pass is necessary. It is not as straightforward as for
///             fixed allocations.
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
class ULIS_API FShrinkableAllocArena {
private:
    friend class FShrinkableAllocMemoryPool;

private:
    /////////////////////////////////////////////////////
    /// @class      FIterator
    /// @brief      The FShrinkableAllocArena::FIterator class is a utility helper
    ///             class useful to private parts of FShrinkableAllocArena only,
    ///             in order to avoid error-prone redundancies in pointer arithmetics.
    class FIterator
    {
    public:
        // Ctors
        explicit FIterator( tMetaBase iMetaBase );
        explicit FIterator( tClient iClient );
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
        // Deviation from  The Three Basic Rules of Operator Overloading in C++
        // The Decision between Member and Non-member
        // Cause: compactness and private imp
        FIterator operator+( int iValue ) const;
        FIterator operator-( int iValue ) const;
        bool operator==( const FIterator& iOther ) const;
        bool operator!=( const FIterator& iOther ) const;
        uint32 PrevSize() const;
        uint32 NextSize() const;
        tClient Client();
        const tClient Client() const;
        tMetaBase MetaBase();
        const tMetaBase MetaBase() const;
        tAlloc Allocation();
        const tAlloc Allocation() const;
        bool IsFree() const;
        bool IsUsed() const;
        bool IsValid() const;
        bool IsBegin() const;
        bool IsEnd() const;
        void CleanupMetaBase();
        void FillMetaBase();
        void SetPrevSize( uint32 iValue );
        void SetNextSize( uint32 iValue );
        void FreeClient();
        void ResyncClient();
        tClient AllocClient();
        static bool MergeFree( const FIterator& iA, const FIterator& iB );

    private:
        /*!
            This mMetaBase member of type uint8_t* will point to parts of the buffer that is passed as an argument
            to the constructor of the FIterator instance. Although the type is uint8_t*, it will remaine valid as
            a way to iterate through any buffer initially passed as a void* type. The uint8_t* type allows to use
            pointer arithmetics directly on it without additional reinterpret casts, and uint8_t* increments in steps
            of one byte.
        */
        mutable tMetaBase mMetaBase; ///< Pointer in arena block.
    };

public:
    /*!
        Destructor, destroy the arena.
        Cleanup the underlying arena block.
        Make sure all allocations are free before destroying an arena, or it will trigger an assert in debug builds.
        You can use UnsafeFreeAll() before destruction to ensure it doesn't crash, but only if you know what you're doing.
        It is possible to do so before program termination, but if clients don't expect this, they will all be deleted
        without notification and induce a corrupted state within the program.
    */
    ~FShrinkableAllocArena();

    /*!
        Constructor from arena size and alloc size.
        Make sure to chose an arena size that is greater than the alloc size and that can fit an expected number of
        fixed cells allocations.
        It is best to chose an alloc size that is a divisor of arena size, the behaviour is undefined otherwise.
        Also, take into account that the block size will use space for the meta data, so the number of objects that can
        fit is actually smaller than expected, even if max alloc is a divisor of arena size. You can compute the extra meta
        paddings applied for a given number of objects with dedicated methods or see other constructors that will meet these
        expectations.
    */
    explicit
    FShrinkableAllocArena(
          byte_t iArenaSize
        , byte_t iMaxAllocSize
    );

    /*!
        Constructor from alloc size and number of expected cells or allocs.
        An arena size will be coomputed so that it can fit an expected number of
        fixed cells allocations, that is a proper arena size for the buffers plus extra meta pad size.
    */
    explicit
    FShrinkableAllocArena(
          byte_t iMaxAllocSize
        , uint64 iNumCells
    );

    /*! Explicitely deleted copy constructor */
    FShrinkableAllocArena( const FShrinkableAllocArena& ) = delete;

    /*! Explicitely deleted copy assignment operator */
    FShrinkableAllocArena& operator=( const FShrinkableAllocArena& ) = delete;



public:
    // Public Check API
    /*! Checks wether the arena is full or not. */
    bool IsFull() const;

    /*! Checks wether the arena is empty or not. */
    bool IsEmpty() const;



    // Size API
    /*! Obtain the arena size in bytes */
    byte_t ArenaSize() const;

    /*! Obtain the max alloc size in bytes */
    byte_t MaxAllocSize() const;



    // Memory API
    /*! Obtain the total theoretical memory for this pool as bytes, given the current number or arenas. */
    byte_t TotalMemory() const;

    /*! Obtain current free memory as bytes. */
    byte_t FreeMemory() const;

    /*! Obtain current used memory as bytes. */
    byte_t UsedMemory() const;



    // Alloc API
    /*!
        Obtain an client to an allocation within this arena.
        If full or a failure occurs, returns nullptr.
    */
    tClient Malloc( byte_t iAllocSize = ULIS_UINT32_MAX );

    /*!
        Free an allocation and its associated client.
        It doesn't need to be resident in pool.
    */
    static void Free( tClient iClient );

    /*!
        Free all resident allocations in this arena.
        Clients are deleted and not notified about their status.
        This is unsafe and dangerous, unless you're done with all clients.
    */
    void UnsafeFreeAll();

    /*!
        Shrink an alloc and return if succesful or not. If not succesfull, the alloc remains as before and is still valid, it just didn't shrink.
    */
    static bool Shrink( tClient iClient, byte_t iNewSize );


    // Fragmentation API
    /*! Get the local frag estimation. */
    float LocalFragmentation() const;

    /*! Trigger local defragmentation. */
    void DefragSelfForce();



    // Debug API
    /*! Get a textual representation of the arena for debug purposes. */
    void DebugPrint( int iType = 0, int iCol = 100 ) const;



private:
    // Private Size API
    uint8* LowBlockAdress();
    uint8* HighBlockAdress();
    const uint8* LowBlockAdress() const;
    const uint8* HighBlockAdress() const;
    uint64 BlockSize() const;
    bool IsResident( tMetaBase iMetaBase );

    // Private Memory API
    FIterator FindFirstMinAlloc( bool iUsed, byte_t iMinimumSizeBytes = ULIS_UINT32_MAX, const FIterator& iFrom = FIterator::MakeNull() ); // default max clamped to MaxAllocSize, default from at mBlock ( LowAdress )
    FIterator FindLastMinAlloc( bool iUsed, byte_t iMinimumSizeBytes = ULIS_UINT32_MAX, const FIterator& iFrom = FIterator::MakeNull() ); // default max clamped to MaxAllocSize, default from at mBlock ( LowAdress )
    void Initialize();
    static uint64 InitialFreeMemory( uint64 iArenaSize );
    static void MoveAlloc( FIterator& iFrom, FIterator& iTo,  uint64* oFreed = nullptr, uint64* oUsed = nullptr );

    // Iterator API
    FIterator Begin();
    FIterator End();
    const FIterator Begin() const;
    const FIterator End() const;

private:
    // Private Data Members
    const uint64 mArenaSize; ///< Arena Size in bytes, with extra meta pad for cell
    const uint32 mMaxAllocSize; ///< Max allocation Size in bytes, without extra meta pad for cell
    tByte* const mBlock; ///< Underlying arena storage buffer with allocation data and meta infos [meta][data] ...

    static constexpr const uint8 smMetaPrevDeltaPad = sizeof( tClient ); ///< Constant padding for meta base storage: prev delta pad.
    static constexpr const uint8 smMetaNextDeltaPad = smMetaPrevDeltaPad + sizeof( uint32 ); ///< Constant padding for meta base storage: next delta pad.
    static constexpr const uint8 smMetaTotalPad = smMetaNextDeltaPad + sizeof( uint32 ); ///< Constant padding for meta base storag: total pad.
};

ULIS_NAMESPACE_END

