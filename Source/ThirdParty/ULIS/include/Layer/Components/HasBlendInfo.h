// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasBlendInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasBlendInfo class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/BlendInfo.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnBlendInfoChanged, void, const FBlendInfo& )
/////////////////////////////////////////////////////
/// @class      IHasBlendInfo
/// @brief      Simple PIC Blendable class.
struct ULIS_API IHasBlendInfo
    : public FOnBlendInfoChanged
{
protected:
    // CTor
    IHasBlendInfo(
          eBlendMode iBlendMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.f
        , const FOnBlendInfoChanged& iDelegate = FOnBlendInfoChanged()
    );

public:
    // Getters
    eBlendMode BlendMode() const;
    eAlphaMode AlphaMode() const;
    ufloat Opacity() const;

    // Setters
    void SetBlendMode( eBlendMode iValue );
    void SetAlphaMode( eAlphaMode iValue );
    void SetOpacity( ufloat iValue );

private:
    // Data
    FBlendInfo mInfo;
};

ULIS_NAMESPACE_END

