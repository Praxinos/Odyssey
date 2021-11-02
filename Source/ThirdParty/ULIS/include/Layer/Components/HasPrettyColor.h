// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasPrettyColor.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasPrettyColor class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Color.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnColorChanged, void, const FColor& )
/////////////////////////////////////////////////////
/// @class      IHasPrettyColor
/// @brief      Simple HasPrettyColor class.
class ULIS_API IHasPrettyColor
    : public FOnColorChanged
{
protected:
    IHasPrettyColor(
          const FColor& iColor = FColor::Transparent
        , const FOnColorChanged& iDelegate = FOnColorChanged()
    );

public:
    const FColor& PrettyColor() const;
    void SetPrettyColor( const FColor& iValue );

private:
    FColor mColor;
};

ULIS_NAMESPACE_END

