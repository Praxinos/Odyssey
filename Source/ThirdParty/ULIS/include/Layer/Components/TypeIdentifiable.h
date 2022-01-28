// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TypeIdentifiable.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the ITypeIdentifiable class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "String/String.h"
#include "String/CRC32.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      ITypeIdentifiable
/// @brief      Simple TypeIdentifiable class.
class ULIS_API ITypeIdentifiable
{
public:
    // ITypeIdentifiable Interface
    virtual const FString Type() const = 0;
    virtual const uint32 TypeID() const = 0;

    #define ULIS_OVERRIDE_TYPEID_INTERFACE( __ID__ )                                \
        constexpr static const char* StaticType() { return  __ID__ ; }              \
        constexpr static const uint32 StaticTypeID() { return  crc32b( __ID__ ); }  \
        virtual const FString Type() const override { return  StaticType(); }       \
        virtual const uint32 TypeID() const override { return  StaticTypeID(); }
};

ULIS_NAMESPACE_END

