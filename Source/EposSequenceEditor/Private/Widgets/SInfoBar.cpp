// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Widgets/SInfoBar.h"

//#include "Brushes/SlateColorBrush.h"

#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "SInfoBar"

//---

void
SInfoBar::Construct( const FArguments& InArgs )
{
    ChildSlot
    .Padding( InArgs._Padding )
    [
        SNew( SBorder )
        .VAlign( VAlign_Center )
        .Padding( FMargin( 5.f, 2.f ) )
        .BorderImage( FEditorStyle::GetNoBrush() )
        [
            SNew( STextBlock )
            .Text( InArgs._Text )
            .TextStyle( &FEposSequenceEditorStyle::Get()->GetWidgetStyle<FTextBlockStyle>( "EposSequenceEditor.ToolBar.Heading" ) )
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
