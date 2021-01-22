// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionCamera"

//---

//static
float
SCinematicBoardSectionCamera::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return FEditorStyle::GetFontStyle( "NormalFont" ).Size + 8.f;
}

void SCinematicBoardSectionCamera::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    static FSlateColorBrush brush = FSlateColorBrush( FLinearColor( .15f, .06f, .14f ) );

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( &brush )
        //.Padding( 5 )
        [
            SNew( STextBlock )
            .Text( FText::FromString( mBinding.GetName() ) )
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
