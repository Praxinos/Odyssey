// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         UserData.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IUserData class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "TypeIdentifiable.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      IUserData
/// @brief      Simple UserData class.
class ULIS_API IUserData
    : public ITypeIdentifiable
{
public:
    virtual ~IUserData() = 0;
};

/////////////////////////////////////////////////////
/// @class      FTextUserData
/// @brief      Simple Test UserData class.
class ULIS_API FTextUserData
    : public IUserData
{
public:
    virtual ~FTextUserData() {}
    FTextUserData( const FString& iValue )
        : mData( iValue )
    {}

public:
    // FTextUserData Interface
    const FString& Text() const {
        return  mData;
    }

    void SetText( const FString& iValue ) {
        mData = iValue;
    }

    // ITypeIdentifiable Interface
    ULIS_OVERRIDE_TYPEID_INTERFACE( "Text_UserData" ); // Should be unique amongst other UserData types !

private:
    FString mData;
};

ULIS_NAMESPACE_END

