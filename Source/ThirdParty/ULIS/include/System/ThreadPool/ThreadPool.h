// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ThreadPool.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FThreadPool class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
class FThreadPool_Private;
/////////////////////////////////////////////////////
/// @class      FThreadPool
/// @brief      The FThreadPool class provides a way to hold a thread pool with
///             several threads running and waiting for commands, with no overhead
///             related to thread launching and joining.
/// @details    The FThreadPool holds a certain number of threads which actual
///             implementation is irrelevant ( wether the backend is Posix threads,
///             Intel TBB, or no thread support at all ). If the system doesn't
///             support multithreading, the FThreadPool emulates a multithreaded
///             behaviour anyway with no change to application logic, but it
///             processes it in a linear monothreaded fashion.
///
///             \sa FCPUInfo
///             \sa FCommandQueue
class ULIS_API FThreadPool
{
    friend class FCommandQueue_Private;

public:
    ~FThreadPool();
    FThreadPool( uint32 iNumWorkers = MaxWorkers() );
    FThreadPool( const FThreadPool& ) = delete;
    FThreadPool& operator=( const FThreadPool& ) = delete;
    void WaitForCompletion();
    void SetNumWorkers( uint32 iNumWorkers );
    uint32 GetNumWorkers() const;
    static uint32 MaxWorkers();

private:
    FThreadPool_Private* d;
};

ULIS_NAMESPACE_END

