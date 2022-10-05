// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasText.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasText class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/TextInfo.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
ULIS_DECLARE_SIMPLE_DELEGATE( FOnTextInfoChanged, void, const FTextInfo& )
/////////////////////////////////////////////////////
/// @class      IHasText
/// @brief      Simple HasText class.
struct ULIS_API IHasText
    : public FOnTextInfoChanged
{
protected:
    IHasText(
          const FWString& iString = L"Lorem ipsum dolor sit amet..."
        , const FFont& iFont = FFont::DefaultFont
        , int iSize = 12
        , const FColor& iColor = FColor::Black
        , bool iAntiAliased = true
        , const FOnTextInfoChanged& iDelegate = FOnTextInfoChanged()
    );

public:
    FWString Text() const;
    const FFont& Font() const;
    int FontSize() const;
    const FColor& TextColor() const;
    bool IsAntiAliased() const;

    void SetText( const FWString& iValue );
    void SetFont( const FFont& iValue );
    void SetFontSize(  int iValue );
    void SetTextColor( const FColor& iValue );
    void SetAntiAliased( bool iValue );

private:
    FTextInfo mInfo;
};

ULIS_NAMESPACE_END

