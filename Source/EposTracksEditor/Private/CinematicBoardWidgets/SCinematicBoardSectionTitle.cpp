// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionTitle"

//---

//static
float
SCinematicBoardSectionTitle::GetHeight( TSharedRef<const FCinematicBoardSection> iBoardSection )
{
    return SequencerSectionConstants::DefaultSectionHeight + 5.f;
}

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mName = InArgs._Name;

    ChildSlot
    .HAlign( EHorizontalAlignment::HAlign_Center )
    [
        SNew( SBox )
        .HeightOverride( GetHeight( iBoardSection ) )
        [
            SAssignNew( mWidgetName, SInlineEditableTextBlock )
            .Text( mName )
            .ColorAndOpacity( FLinearColor( .75f, .75f, .75f ) )
            .ShadowOffset( FVector2D( 1, 1 ) )
            .OnTextCommitted( mBoardSection.ToSharedRef(), &FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted )
        ]
    ];
}

int32
SCinematicBoardSectionTitle::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    static const FSlateBrush* background_brush = FEditorStyle::GetBrush( "ToolPanel.GroupBorder" );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        background_brush,
        ESlateDrawEffect::None,
        background_brush->GetTint( InWidgetStyle ) * FLinearColor( .5f, .5f, .5f ) // Same grey as TimeSlider widget
    );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

void
SCinematicBoardSectionTitle::EnterRename()
{
    mWidgetName->EnterEditingMode();
}

//---

#undef LOCTEXT_NAMESPACE
