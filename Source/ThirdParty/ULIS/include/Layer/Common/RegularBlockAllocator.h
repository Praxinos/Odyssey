// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         RegularBlockAllocator.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FRegularBlockAllocator class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FRegularBlockAllocator
/// @brief      Allocator Utility for FBlock
class ULIS_API FRegularBlockAllocator
{
private:
    FRegularBlockAllocator() = default;

public:
    static FBlock* New( uint16 iWidth, uint16 iHeight, eFormat iFormat, const FColorSpace* iColorSpace );
    static void Delete( FBlock* iBlock );
};

ULIS_NAMESPACE_END

