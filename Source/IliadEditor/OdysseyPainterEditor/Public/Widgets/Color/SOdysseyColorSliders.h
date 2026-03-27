// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include <ULIS>

#include "SOdysseyColorSlider.h"

class ODYSSEYPAINTEREDITOR_API SOdysseyColorSliders : public SCompoundWidget
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
    DECLARE_DELEGATE_TwoParams( FOnColorChanged, eOdysseyEventState::Type, const ::ULIS::FColor& );

public:
    SLATE_BEGIN_ARGS( SOdysseyColorSliders )
        {}
    SLATE_ATTRIBUTE(::ULIS::FColor, Color)
    SLATE_EVENT( FOnColorChanged, OnColorChanged )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    void Construct(const FArguments& InArgs);

public:
    // Public Callbacks
    // void SetColor( const ::ULIS::FColor& iColor );



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
            .OnColorChanged(OnColorChanged)
            .Visibility_Lambda([sliderOption]() { return sliderOption->enabled ? EVisibility::Visible : EVisibility::Collapsed; });

        return MakeShareable(sliderOption);
    }

private:
    // Private data members
    TAttribute<::ULIS::FColor> mColor;
    TArray< FSliderOptionItem > sliders_options;
    TSharedPtr< SComboButton > combo_button;
    TSharedPtr< SVerticalBox > combo_menu;
    TSharedPtr< SScrollBox > contents;
    FOnColorChanged OnColorChanged;
};
