// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         SchedulePolicy.h
* @author       Clement Berthaud && Eric Scholl
* @brief        This file provides declaration for the FSchedulePolicy class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
enum eScheduleTimePolicy : uint8 {
      ScheduleTime_Sync = 0
    , ScheduleTime_Async = 1
};

enum eScheduleRunPolicy : uint8 {
      ScheduleRun_Mono  = 0
    , ScheduleRun_Multi = 1
};

enum eScheduleModePolicy : uint8 {
      ScheduleMode_Scanlines = 0
    , ScheduleMode_Chunks = 1
};

enum eScheduleParameterPolicy : uint8 {
      ScheduleParameter_Count = 0
    , ScheduleParameter_Length = 1
};

/////////////////////////////////////////////////////
/// @class      FSchedulePolicy
/// @brief      The FSchedulePolicy class provides a way to control how a task
///             is dispatched and scheduled on a multithreaded system, in
///             coordination with a FThreadPool and a FCommandQueue.
/// @details    The FSchedulePolicy specifies wether a task should be
///             multithreaded or not, and if so which policy should be applied.
///             Wether the scheduling is done on a chunk basis, or on a scanline
///             basis, for example.
///
///             \sa FContext
///             \sa FThreadPool
///             \sa FCPUInfo
///             \sa FCommandQueue
class ULIS_API FSchedulePolicy
{
public:
    /*! Destructor */
    ~FSchedulePolicy();

    /*!
        Constructor with default values.
        Defaults to mono scanline run and synchronous.
    */
    FSchedulePolicy(
          eScheduleTimePolicy iTime = ScheduleTime_Sync
        , eScheduleRunPolicy iRun = ScheduleRun_Multi
        , eScheduleModePolicy iMode = ScheduleMode_Scanlines
        , eScheduleParameterPolicy iParam = ScheduleParameter_Count
        , uint64 iValue = 0
    );

    /*! Getter for the Time Value. */
    eScheduleTimePolicy TimePolicy() const;

    /*! Getter for the Run Policy. */
    eScheduleRunPolicy RunPolicy() const;

    /*! Getter for the Mode Policy. */
    eScheduleModePolicy ModePolicy() const;

    /*! Getter for the Parameter Policy. */
    eScheduleParameterPolicy ParameterPolicy() const;

    /*! Getter for the Parameter Value. */
    uint64 Value() const;

public:
    static const FSchedulePolicy AsyncCacheEfficient;
    static const FSchedulePolicy CacheEfficient;
    static const FSchedulePolicy AsyncMonoChunk;
    static const FSchedulePolicy MonoChunk;
    static const FSchedulePolicy AsyncMultiScanlines;
    static const FSchedulePolicy AsyncMonoScanlines;
    static const FSchedulePolicy MultiScanlines;
    static const FSchedulePolicy MonoScanlines;

private:
    eScheduleTimePolicy         mTime;
    eScheduleRunPolicy          mRun;
    eScheduleModePolicy         mMode;
    eScheduleParameterPolicy    mParameter;
    uint64                      mValue;
};

ULIS_NAMESPACE_END

