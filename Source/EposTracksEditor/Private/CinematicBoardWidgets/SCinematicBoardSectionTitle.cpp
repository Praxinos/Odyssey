// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionTitle.h"

#include "Brushes/SlateColorBrush.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionTitle"

//---

void
SCinematicBoardSectionTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    ChildSlot
    .HAlign( EHorizontalAlignment::HAlign_Center )
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign( EHorizontalAlignment::HAlign_Center )
        .Padding( 0.f, 4.f )
        [
            SAssignNew( mWidgetName, SInlineEditableTextBlock )
            .Text_Lambda( [this] { return HandleText(); } )
            .ColorAndOpacity_Lambda( [this] { return HandleTextColor(); } )
            .ShadowOffset( FVector2D( 1, 1 ) )
            .OnTextCommitted( mBoardSection.Pin().ToSharedRef(), &FCinematicBoardSection::HandleThumbnailTextBlockTextCommitted )
        ]
    ];
}

FText
SCinematicBoardSectionTitle::HandleText() const
{
    if( !mBoardSection.IsValid() )
        return FText::GetEmpty();

    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    FText section_text = mBoardSection.Pin()->HandleThumbnailTextBlockText();
    FText sequence_text = subsection.GetSequence() ? subsection.GetSequence()->GetDisplayName() : FText::GetEmpty();

    return !section_text.IsEmpty()
        ?
        section_text
        :
        FText::Format( FText::FromString( "<{0}>" ), sequence_text );
}

FLinearColor
SCinematicBoardSectionTitle::HandleTextColor() const
{
    if( !mBoardSection.IsValid() )
        return FLinearColor::White;

    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    FText section_text = mBoardSection.Pin()->HandleThumbnailTextBlockText();
    //FText sequence_text = subsection.GetSequence() ? subsection.GetSequence()->GetDisplayName() : FText::GetEmpty();

    return !section_text.IsEmpty()
        ?
        FLinearColor( .75f, .75f, .75f )
        :
        FLinearColor( .25f, .25f, .25f );
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
