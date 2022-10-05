// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Event.h
* @author       Clement Berthaud && Eric Scholl
* @brief        This file provides declaration for the FEvent class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/Callback.h"

ULIS_NAMESPACE_BEGIN
class FEvent_Private;

/////////////////////////////////////////////////////
// eEventStatus
enum eEventStatus : uint8
{
      EventStatus_Idle //Created, Not flushed, No jobs ready
    , EventStatus_Queued //Created, flushed, No jobs ready
    , EventStatus_Finished //jobs donc, children informed
};

/////////////////////////////////////////////////////
/// @class      FEvent
/// @brief      The FEvent class provides a way to get asynchronous status
///             information about how a task is being processed, once it has been
///             dispatched and scheduled on a multithreaded system, in
///             coordination with a FThreadPool and a FCommandQueue.
/// @details    The FEvent allows to get a handle of the chosen policy for
///             a given task, and wether it has been completed or not. It is used
///             in conjunction with FThreadPool, FSchedulePolicy, FCommandQueue
///             and FContext.
///
///             \sa FContext
///             \sa FSchedulePolicy
///             \sa FThreadPool
///             \sa FCPUInfo
///             \sa FCommandQueue
class ULIS_API FEvent
{
    friend class FInternalEvent;
    friend class FContext;
    friend class FCommand;

public:
    /*! Destructor */
    ~FEvent();

    /*! Constructor */
    FEvent( const FOnEventComplete& iOnEventComplete = FOnEventComplete() );

    FEvent( const FEvent& );
    FEvent& operator=( const FEvent& );

    eEventStatus Status() const;
    void Wait() const;

    static FEvent NoOP();

private:
    FEvent_Private* d;
};

ULIS_NAMESPACE_END

