// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Types/SlateStructs.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Layout/Visibility.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Layout/Children.h"
#include "Widgets/SPanel.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboBox.h"

#include "SOdysseyLeafWidget.h"

#include "Color/SOdysseyAdvancedColorWheel.h"
#include <ULIS>

class ODYSSEYWIDGETS_API SOdysseyColorSelector : public SCompoundWidget
{
    typedef SCompoundWidget tSuperClass;

public:
    SLATE_BEGIN_ARGS( SOdysseyColorSelector )
    {}
        SLATE_ATTRIBUTE( ::ULIS::FColor, Color )
        SLATE_EVENT( FOnColorChange, OnColorChange )
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
    TSharedPtr< SOdysseyAdvancedColorWheel > mAdvancedColorWheel;
    TSharedPtr< SEditableTextBox > mHexTextBox;

    TAttribute<::ULIS::FColor> mColor;
    FOnColorChange mOnColorChangeCallback;
};
