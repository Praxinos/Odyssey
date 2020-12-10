// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    mColor = InArgs._Color;
    OnColorChangeCallback = InArgs._OnColorChange;
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
/* void
SOdysseyColorSliders::SetColor( const ::ul3::FPixelValue& iColor )
{
    if( bDisableNextCallback ) {
        bDisableNextCallback = false;
        return;
    }

    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        sliders_options[i]->widget->SetColor( iColor );
    }
} */


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

void
SOdysseyColorSliders::GenerateMenu()
{
    sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_RGB>( "RGB", true ) );
    sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_HSV>( "HSV", true ) );
    sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_HSL>( "HSL", false ) );
    sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_CMYK>( "CMYK", false ) );
    //sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_YUV>( "YUV", false ) );
    sliders_options.Add( GenerateSliderOption<FOdysseyGroupChannelSlider_Lab>( "LabD65", true ) );

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
    // GenerateContents();
}


void
SOdysseyColorSliders::GenerateContents()
{
    //contents->ClearChildren();
    for( int i = 0; i < sliders_options.Num(); ++i )
    {
        //if( !sliders_options[i]->enabled )
            //continue;

        contents->AddSlot()
            .Padding( 2, 2, 2, 4 )
            [
                sliders_options[i]->widget.ToSharedRef()
            ];
    }
}

#undef LOCTEXT_NAMESPACE

