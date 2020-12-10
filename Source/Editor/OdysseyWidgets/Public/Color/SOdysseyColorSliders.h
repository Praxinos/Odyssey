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
#include "Widgets/Layout/SScrollBox.h"
#include <ULIS3>

#include "SOdysseyColorSlider.h"

//class IOdysseyGroupChannelSlider;
//DECLARE_DELEGATE_OneParam( FOnColorChanged, const ::ul3::FPixelValue& );

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
    SLATE_ATTRIBUTE(::ul3::FPixelValue, Color)
    SLATE_EVENT( FOnColorChange, OnColorChange )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    // Public Callbacks
    // void SetColor( const ::ul3::FPixelValue& iColor );



private:
    // Private Callbacks
    void GenerateMenu();
    void ItemChanged( int index, ECheckBoxState iState );
    void GenerateContents();

    template<class T>
    FSliderOptionItem
    GenerateSliderOption(FString iName, bool iEnabled)
    {
        FSliderOption* sliderOption = new FSliderOption(iName, iEnabled, nullptr);

        sliderOption->widget = SNew(T/*FOdysseyGroupChannelSlider_RGB*/)
            .HeightOverride(20)
            .Color(mColor)
            .OnColorChange(OnColorChangeCallback)
            .Visibility_Lambda([sliderOption]() { return sliderOption->enabled ? EVisibility::Visible : EVisibility::Collapsed; });

        return MakeShareable(sliderOption);
    }

private:
    // Private data members
    TAttribute<::ul3::FPixelValue> mColor;
    TArray< FSliderOptionItem > sliders_options;
    TSharedPtr< SComboButton > combo_button;
    TSharedPtr< SVerticalBox > combo_menu;
    TSharedPtr< SScrollBox > contents;
    FOnColorChange OnColorChangeCallback;
};
