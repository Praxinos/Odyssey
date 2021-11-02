// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Memory.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the memory functions.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
ULIS_API void* XMalloc( uint64 iSizeBytes );
ULIS_API void XFree( void* iAlloc );

template< typename Derived, typename Base >
Derived& DynamicCast( Base& iBase );

/*
class UBase {
public:
    virtual ~UBase() {
    }
};
class UDerived : public UBase {};

template ULIS_API UDerived& DynamicCast< UDerived, UBase >( UBase& );
*/

ULIS_NAMESPACE_END

