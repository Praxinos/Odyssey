// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "OdysseyEventState.h"

#include <ULIS>

class SOdysseyColorHexadecimal : public SCompoundWidget
{
    typedef SCompoundWidget tSuperClass;

public:
    DECLARE_DELEGATE_TwoParams( FOnColorChanged, eOdysseyEventState::Type, const ::ULIS::FColor& );

public:
    SLATE_BEGIN_ARGS( SOdysseyColorHexadecimal )
    {}
        SLATE_ATTRIBUTE( ::ULIS::FColor, Color )
        SLATE_EVENT( FOnColorChanged, OnColorChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

private:
    // Private Callbacks
    bool HexBoxIsValidChar( TCHAR iChar ) const;
    FReply HexBoxOnKeyChar( const FGeometry&, const FCharacterEvent& iEvent ) const;
    void HexBoxOnTextChanged( const FText& iText );
    void HexBoxOnTextCommited( const FText&, ETextCommit::Type );
    FText GetColorHex() const;

    FText GetHexText() const;
    void OnHexTextChanged( const FText& iText );
    void OnHexTextCommitted(const FText& Text, ETextCommit::Type CommitType);

private:
    // Private data members
    TSharedPtr< SEditableTextBox > mHexTextBox;

    TAttribute<::ULIS::FColor> mColor;
    FOnColorChanged mOnColorChanged;
};
