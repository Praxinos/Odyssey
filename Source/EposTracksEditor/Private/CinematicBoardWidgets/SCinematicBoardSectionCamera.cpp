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
    return SequencerSectionConstants::DefaultSectionHeight + 5.f;
}

void
SCinematicBoardSectionCamera::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
        .HeightOverride( GetHeight( iBoardSection ) )
        //[
        //    SNew( STextBlock )
        //    .Text( FText::FromString( mBinding.GetName() ) )
        //]
    ];
}


static
FTimeToPixel
ConstructTimeConverterForSection2( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
}


int32
SCinematicBoardSectionCamera::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .15f, .06f, .14f ) );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        &background_brush,
        ESlateDrawEffect::None,
        background_brush.GetTint( InWidgetStyle )
    );

    //---

    TArray<double> keys = mBoardSection->GetKeys();

    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( auto key : keys )
    {
        const FVector2D KeySize = SequencerSectionConstants::KeySize;
        //static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection2( AllottedGeometry, *mBoardSection->GetSectionObject() ).SecondsToPixel( key );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            CircleKeyBrush
        );
    }

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
