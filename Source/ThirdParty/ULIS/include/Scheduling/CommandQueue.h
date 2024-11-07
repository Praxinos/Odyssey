// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CommandQueue.h
* @author       Clement Berthaud && Eric Scholl
* @brief        This file provides the declaration for the FCommandQueue class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
class FCommand;
class FCommandQueue_Private;
/////////////////////////////////////////////////////
/// @class      FCommandQueue
/// @brief      The FCommandQueue class provides a way to enqueue tasks for being
///             processed asynchronously in coordination with a FThreadPool
/// @details    The FCommandQueue stores a TQueue of FCommand and schedules the
///             commands on the FThreadPool.
///
///             \sa FCommand
///             \sa FThreadPool
class ULIS_API FCommandQueue
{
    friend class FContext;

public:
    /*! Destructor */
    ~FCommandQueue();

    /*! Constructor */
    FCommandQueue( FThreadPool& iPool );

    FCommandQueue( const FCommandQueue& ) = delete;
    FCommandQueue& operator=( const FCommandQueue& ) = delete;

public:
    /*!
        Issue all commands and return immediately.
    */
    void Flush();

    /*!
        Issue all commands and wait for completion
    */
    void Finish();

    /*!
        Wait for completion of all already issued commands
    */
    void Fence();

private:
    FCommandQueue_Private* d;
};

ULIS_NAMESPACE_END

