// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         FormatInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FormatInfo utilities.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FFormatInfo
struct ULIS3_API FFormatInfo
{
    ~FFormatInfo();
    FFormatInfo( tFormat iFMT );
    FFormatInfo( const FFormatInfo& iOther );
    FFormatInfo( FFormatInfo&& iOther );
    FFormatInfo& operator=( const FFormatInfo& iOther );

    tFormat     FMT;    // Format
    eType       TP;     // Type
    eColorModel CM;     // Color Model
    uint8*      IDT;    // Index Table
    uint8       BPC;    // Bytes Per Channel
    uint8       NCC;    // Num Color Channels
    uint8       HEA;    // Has Extra Alpha
    uint8       COD;    // ReverseSwapped Code
    uint8       SPP;    // Samples Per Pixel
    uint8       BPP;    // Bytes Per Pixel
    uint8       AID;    // Alpha Index
    uint8       REV;    // Reversed
    uint8       SWA;    // Swapped
};

ULIS3_NAMESPACE_END

