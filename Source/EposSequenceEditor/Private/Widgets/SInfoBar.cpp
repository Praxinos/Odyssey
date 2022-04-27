// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
