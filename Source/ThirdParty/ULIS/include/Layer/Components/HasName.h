// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasName.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasName class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "String/String.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnNameChanged, void, const FString& )
/////////////////////////////////////////////////////
/// @class      IHasName
/// @brief      Simple HasName class.
class ULIS_API IHasName
    : public FOnNameChanged
{
public:
    IHasName(
          const FString& iValue
        , const FOnNameChanged& iDelegate = FOnNameChanged()
    );

public:
    const FString& Name() const;
    void SetName( const FString& iValue );

private:
    FString mStr;
};

ULIS_NAMESPACE_END

