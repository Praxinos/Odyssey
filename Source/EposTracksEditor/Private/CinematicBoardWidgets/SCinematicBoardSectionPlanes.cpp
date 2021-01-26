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
    return SequencerSectionConstants::DefaultSectionHeight + 5.f;
}

void
SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
        .HeightOverride( GetHeight( iBoardSection ) )
        [
            SNew( STextBlock )
            .Text( FText::FromString( mBinding.GetName() ) )
        ]
    ];
}

int32
SCinematicBoardSectionPlane::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        &background_brush,
        ESlateDrawEffect::None,
        background_brush.GetTint( InWidgetStyle )
    );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---

//static
float
SCinematicBoardSectionPlanes::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return iBoardSection->GetMaxPlaneBindings() * SCinematicBoardSectionPlane::GetHeight( iBoardSection );
}

void
SCinematicBoardSectionPlanes::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
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
