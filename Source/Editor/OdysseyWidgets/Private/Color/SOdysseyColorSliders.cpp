// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "Color/SOdysseyColorSliders.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SCheckBox.h"
#include "Brushes/SlateColorBrush.h"
#include "Widgets/Views/SListView.h"
#include "Color/SOdysseyColorSlider.h"

#define LOCTEXT_NAMESPACE "OdysseyColorSliders"


/////////////////////////////////////////////////////
// Cosmetic
static FSlateColorBrush sliders_header_brush = FSlateColorBrush( FLinearColor( FColor( 70, 70, 70 ) ) );


/////////////////////////////////////////////////////
// SOdysseyColorSliders
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyColorSliders::Construct( const FArguments& InArgs )
{
    OnColorChangedCallback = InArgs._OnColorChanged;
    ChildSlot
    [
        SNew( SVerticalBox )
        +SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            +SHorizontalBox::Slot()
            [
                SNew( SBorder )
                .BorderImage( &sliders_header_brush )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "Sliders", "Sliders" ) )
                ]
            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SAssignNew( combo_button, SComboButton )
            ]
        ]
        +SVerticalBox::Slot()
        [
            SAssignNew( contents, SScrollBox )
        ]
    ];

    GenerateMenu();
    combo_button->SetMenuContent( combo_menu.ToSharedRef() );
    GenerateContents();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Public Callbacks
void
SOdysseyColorSliders::SetColor( const ::ULIS::CColor& iColor )
{
    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        sliders_options[i]->widget->SetColor( iColor );
    }
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks
void
SOdysseyColorSliders::GenerateMenu()
{
    sliders_options.Add( MakeShared< FSliderOption >( "RGB",    true,   SNew( FOdysseyGroupChannelSlider_RGB  ).HeightOverride( 20 ).OnColorChanged( this, &SOdysseyColorSliders::HandleColorChanged ) ) );
    sliders_options.Add( MakeShared< FSliderOption >( "HSV",    true,  SNew( FOdysseyGroupChannelSlider_HSV  ).HeightOverride( 20 ).OnColorChanged( this, &SOdysseyColorSliders::HandleColorChanged ) ) );
    sliders_options.Add( MakeShared< FSliderOption >( "HSL",    false,  SNew( FOdysseyGroupChannelSlider_HSL  ).HeightOverride( 20 ).OnColorChanged( this, &SOdysseyColorSliders::HandleColorChanged ) ) );
    sliders_options.Add( MakeShared< FSliderOption >( "CMYK",   false,  SNew( FOdysseyGroupChannelSlider_CMYK ).HeightOverride( 20 ).OnColorChanged( this, &SOdysseyColorSliders::HandleColorChanged ) ) );

    combo_menu = SNew( SVerticalBox );
    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        int index = i;
        combo_menu->AddSlot()
            .Padding( 2, 2, 2, 2 )
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                .FillWidth( true )
                [
                    SNew( STextBlock )
                    .MinDesiredWidth( 100 )
                    .Text( FText::FromString( sliders_options[i]->name ) )
                ]
                +SHorizontalBox::Slot()
                .HAlign( HAlign_Right )
                [
                    SNew( SCheckBox )
                    .OnCheckStateChanged_Lambda( [this, index]( ECheckBoxState iCheckBoxState ) { this->ItemChanged( index, iCheckBoxState ); } )
                    .IsChecked( sliders_options[i]->enabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked )
                ]
            ];
    }
}


void
SOdysseyColorSliders::ItemChanged( int index, ECheckBoxState iState )
{
    bool checked = iState == ECheckBoxState::Checked ? true : false;
    sliders_options[index]->enabled = checked;
    GenerateContents();
}


void
SOdysseyColorSliders::GenerateContents()
{
    contents->ClearChildren();
    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        if( !sliders_options[i]->enabled )
            continue;

        contents->AddSlot()
            .Padding( 2, 2, 2, 4 )
            [
                sliders_options[i]->widget.ToSharedRef()
            ];
    }
}


void
SOdysseyColorSliders::HandleColorChanged( const ::ULIS::CColor& iColor )
{
    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        sliders_options[i]->widget->SetColor( iColor );
    }
    OnColorChangedCallback.ExecuteIfBound( iColor );
}

#undef LOCTEXT_NAMESPACE

