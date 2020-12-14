// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
#include <ULIS3>

class ODYSSEYWIDGETS_API SOdysseyColorSelector : public SCompoundWidget
{

    typedef SCompoundWidget tSuperClass;
    typedef TSharedPtr<FString> FComboItemType;

public:
    SLATE_BEGIN_ARGS( SOdysseyColorSelector )
        {}
    SLATE_ATTRIBUTE( ::ul3::FPixelValue, Color )
    SLATE_EVENT( FOnColorChange, OnColorChange )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    // Public Callbacks
    //void SetColor( const ::ul3::FPixelValue& iColor );

private:
    // Private Callbacks
    TSharedRef<SWidget> MakeWidgetForOption( FComboItemType InOption );
    void OnSelectionChanged(FComboItemType NewValue, ESelectInfo::Type);
    FText GetCurrentItemLabel() const;
    FMargin GetHexBoxPosition() const;
    FOptionalSize GetHexBoxWidth() const;
    FOptionalSize GetHexBoxHeight() const;
    EVisibility GetHexBoxVisibility() const;
    FSlateFontInfo GetHexFont() const;
    bool HexBoxIsValidChar( TCHAR iChar ) const;
    FReply HexBoxOnKeyChar( const FGeometry&, const FCharacterEvent& iEvent ) const;
    void HexBoxOnTextChanged( const FText& iText );
    void HexBoxOnTextCommited( const FText&, ETextCommit::Type );
    FText GetColorHex() const;

private:
    // Private data members
    TSharedPtr< SOdysseyAdvancedColorWheel > adv_color_wheel;
    TArray< FComboItemType > options;
    FComboItemType CurrentItem;
    TSharedPtr< SEditableTextBox > hex_editable_text_box;

    TAttribute<::ul3::FPixelValue> mColor;
    FOnColorChange mOnColorChangeCallback;
};
