// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Callbacks.h
* @author       Clement Berthaud
* @brief        This file provides declarations for the image callbacks.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
typedef void (*fpInvalidateFunction)( const FBlock* /* block */, void* /* info */, const FRect& /* rect */ );
typedef void (*fpCleanupFunction)( tByte* /* data */, void* /* info */ );
ULIS3_API void OnCleanup_FreeMemory( tByte* iData, void* iInfo );
ULIS3_API void OnCleanup_DoNothing(  tByte* iData, void* iInfo );
struct ULIS3_API FOnInvalid
{
    FOnInvalid();
    FOnInvalid( fpInvalidateFunction iInvalidateFunction, void* iInvalidateInfo = nullptr );
    void ExecuteIfBound( const FBlock* iBlock, const FRect& iRect ) const;

private:
    fpInvalidateFunction    execute;
    void*                   info;
};

struct ULIS3_API FOnCleanup
{
    FOnCleanup();
    FOnCleanup( fpCleanupFunction iCleanupFunction, void* iCleanupInfo = nullptr );
    void ExecuteIfBound( tByte* iData ) const;

private:
    fpCleanupFunction       execute;
    void*                   info;
};

ULIS3_NAMESPACE_END

