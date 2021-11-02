// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ContainerAlgorithms.h
* @author       Clement Berthaud
* @brief        This file provides the definition for basic container functions.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Array.h"

ULIS_NAMESPACE_BEGIN
class ULIS_API FContainerAlgorithms {
public:
    template< typename T >
    static void BubbleSort( TArray< T >& ioArray ) {
        T temp;
        const int64 size = static_cast< int64 >( ioArray.Size() );
        for( int64 i = 0; i < size; ++i ) {
            for( int64 j = size - 1; j > i; --j ) {
                if( ioArray[j] < ioArray [j-1] ) {
                    temp = ioArray[j-1];
                    ioArray[j-1] = ioArray[j];
                    ioArray[j] = temp;
                }
            }
        }
    }
};
ULIS_NAMESPACE_END

