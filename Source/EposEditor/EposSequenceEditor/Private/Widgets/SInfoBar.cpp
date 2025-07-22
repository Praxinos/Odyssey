// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/SInfoBar.h"

//#include "Brushes/SlateColorBrush.h"

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "SInfoBar"

//---

void
SInfoBar::Construct( const FArguments& iArgs )
{
    ChildSlot
    .Padding( iArgs._Padding )
    [
        SNew( SBox )
        .VAlign( VAlign_Center )
        .Padding( FMargin( 5.f, 2.f ) )
        [
            SNew( STextBlock )
            .Text( iArgs._Text )
            .TextStyle( FEposSequenceEditorStyle::Get(), "Sequencer.InfoBar" )
            .OnDoubleClicked( iArgs._OnDoubleClicked )
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
