// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         CPUInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FCPUInfo tools.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
enum ePerformanceIntent : uint32 {
      PerformanceIntent_MEM   = 0b0000
    , PerformanceIntent_SSE   = 0b0001
    , PerformanceIntent_AVX   = 0b0010
    , PerformanceIntent_Max   = 0b1111
};

/// @class      FCPUInfo
/// @brief      The FCPUInfo class provides a mean of fetching informations
///             about the CPU in a single-CPU, multicore environment.
/// @details    FCPUInfo provides functions to fetch wether support for 64
///             bits features or SIMD extensions are supported on the CPU that
///             runs the compiled code, at runtime. It has no relation with the
///             computer that compiled the code in the first place, it is a
///             cross-platform tool used to detecting wether the consumer of
///             the library code is able to rely on the implementations
///             compiled with these SIMD features in mind. Additionnaly, it
///             provides extra information about the CPU vendor, and some
///             information about the OS, in relation to its support for the
///             features sets or for 64bit features, although 64bit is always
///             assumed to be true.
///             FCPUInfo also provides some insight about the hardware such
///             as the number of available cores, or the size of cache lines.
class ULIS_API FCPUInfo
{
private:
    ~FCPUInfo() = delete;
    FCPUInfo() = delete;
    FCPUInfo( const FCPUInfo& ) = delete;
    FCPUInfo( FCPUInfo&& ) = delete;

public:
    static bool IsHardwareAMD();
    static bool IsHardwareIntel();
    static bool IsOSx64();
    static bool HasOSAVX();
    static bool HasOSAVX512();
    static bool HasHardwarex64();
    static bool HasHardwareMMX();
    static bool HasHardwareABM();
    static bool HasHardwareRDRAND();
    static bool HasHardwareBMI1();
    static bool HasHardwareBMI2();
    static bool HasHardwareADX();
    static bool HasHardwarePREFETCHWT1();
    static bool HasHardwareMPX();
    static bool HasHardwareSSE();
    static bool HasHardwareSSE2();
    static bool HasHardwareSSE3();
    static bool HasHardwareSSSE3();
    static bool HasHardwareSSE41();
    static bool HasHardwareSSE42();
    static bool HasHardwareSSE4a();
    static bool HasHardwareAES();
    static bool HasHardwareSHA();
    static bool HasHardwareAVX();
    static bool HasHardwareXOP();
    static bool HasHardwareFMA3();
    static bool HasHardwareFMA4();
    static bool HasHardwareAVX2();
    static bool HasHardwareAVX512_F();
    static bool HasHardwareAVX512_PF();
    static bool HasHardwareAVX512_ER();
    static bool HasHardwareAVX512_CD();
    static bool HasHardwareAVX512_VL();
    static bool HasHardwareAVX512_BW();
    static bool HasHardwareAVX512_DQ();
    static bool HasHardwareAVX512_IFMA();
    static bool HasHardwareAVX512_VBMI();
    static uint32 MaxWorkers();
    static uint32 L1CacheSize();
    static uint32 L1CacheLineSize();
};

ULIS_NAMESPACE_END

