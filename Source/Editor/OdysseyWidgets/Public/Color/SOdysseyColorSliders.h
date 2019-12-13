// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
#include "Widgets/Layout/SScrollBox.h"
#include <ULIS_CCOLOR>

class IOdysseyGroupChannelSlider;

DECLARE_DELEGATE_OneParam( FOnColorChanged, const ::ULIS::CColor& );

class ODYSSEYWIDGETS_API SOdysseyColorSliders : public SCompoundWidget
{
    typedef SCompoundWidget tSuperClass;

struct FSliderOption
{
    FString name;
    bool enabled;
    TSharedPtr< IOdysseyGroupChannelSlider > widget;
    FSliderOption( const FString& iName, bool iEnabled, TSharedPtr< IOdysseyGroupChannelSlider > iWidget )
        : name( iName )
        , enabled( iEnabled )
        , widget( iWidget )
    {}
};

    typedef TSharedPtr< FSliderOption > FSliderOptionItem;

public:
    SLATE_BEGIN_ARGS( SOdysseyColorSliders )
        {}
    SLATE_EVENT( FOnColorChanged, OnColorChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    // Public Callbacks
    void SetColor( const ::ULIS::CColor& iColor );

private:
    // Private Callbacks
    void GenerateMenu();
    void ItemChanged( int index, ECheckBoxState iState );

    void GenerateContents();
    void HandleColorChanged( const ::ULIS::CColor& iColor );

private:
    // Private data members
    TArray< FSliderOptionItem > sliders_options;
    TSharedPtr< SComboButton > combo_button;
    TSharedPtr< SVerticalBox > combo_menu;
    TSharedPtr< SScrollBox > contents;
    FOnColorChanged OnColorChangedCallback;
};
