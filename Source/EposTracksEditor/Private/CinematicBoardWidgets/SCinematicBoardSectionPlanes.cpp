// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionPlanes"

//---

//static
float
SCinematicBoardSectionPlane::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return FEditorStyle::GetFontStyle( "NormalFont" ).Size + 8.f;
}

void SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    static FSlateColorBrush brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

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

//---

//static
float
SCinematicBoardSectionPlanes::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return iBoardSection->GetMaxPlaneBindings() * SCinematicBoardSectionPlane::GetHeight( iBoardSection );
}

void SCinematicBoardSectionPlanes::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    TArray<FMovieScenePossessable> possessables( mBoardSection->GetPlaneBindings() );

    TSharedRef<SVerticalBox> planes = SNew( SVerticalBox );
    for( int i = 0; i < possessables.Num(); i++ )
    {
        planes->AddSlot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlane, mBoardSection.ToSharedRef() )
            .Binding( possessables[i] )
        ];
    }

    //---

    ChildSlot
    [
        planes
    ];
}

#undef LOCTEXT_NAMESPACE
