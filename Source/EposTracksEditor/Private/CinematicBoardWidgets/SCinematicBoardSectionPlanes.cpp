// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionPlanes"

//---

void
SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
        [
            SNew( STextBlock )
            .Text( FText::FromString( mBinding.GetName() ) )
        ]
    ];
}

FVector2D
SCinematicBoardSectionPlane::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

int32
SCinematicBoardSectionPlane::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

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

void
SCinematicBoardSectionPlanes::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    TArray<FMovieScenePossessable> possessables( mBoardSection->GetPlaneBindings() );

    TSharedRef<SVerticalBox> planes = SNew( SVerticalBox );
    for( int i = 0; i < mBoardSection->GetMaxPlaneBindings(); i++ )
    {
        planes->AddSlot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlane, mBoardSection.ToSharedRef() )
            .Binding( possessables.IsValidIndex( i ) ? possessables[i] : FMovieScenePossessable() )

            // This doesn't work because this vertical box won't have the same size for all sections
            // and as the height of a track node is getting from the first section in the array (and not necessary the one at the first position in the gui)
            // if the first section has no (or less) planes than others, all planes in the vertical box won't be displayed
            //!possessables.IsValidIndex( i )
            //?
            //SNullWidget::NullWidget
            //:
            //SNew( SCinematicBoardSectionPlane, mBoardSection.ToSharedRef() )
            //.Binding( possessables[i] )
        ];
    }

    //---

    ChildSlot
    [
        planes
    ];
}

#undef LOCTEXT_NAMESPACE
