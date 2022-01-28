// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         MemoryInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FMemoryInfo tools.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/// @class      FMemoryInfo
/// @brief      The FMemoryInfo class provides a mean of fetching informations
///             about the memory or RAM at runtime.
class ULIS_API FMemoryInfo
{
private:
    ~FMemoryInfo() = delete;
    FMemoryInfo() = delete;
    FMemoryInfo( const FMemoryInfo& ) = delete;
    FMemoryInfo( FMemoryInfo&& ) = delete;

public:
    static uint64 TotalVirtualMemory();
    static uint64 VirtualMemoryCurrentlyUsed();
    static uint64 VirtualMemoryCurrentlyUsedByProcess();

    static uint64 TotalRAM();
    static uint64 TotalRAMCurrentlyUsed();
    static uint64 TotalRAMCurrentlyUsedByProcess();

    static udouble CPUCurrentlyUsed();
    static udouble CPUCurrentlyUsedByProcess();

    static uint64 DiskCapacity( const FString& iDisk );
    static uint64 DiskSpaceAvailable( const FString& iDisk );
    static uint64 DiskSpaceFree( const FString& iDisk );
};

ULIS_NAMESPACE_END

