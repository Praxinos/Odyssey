// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Animation/Timeline/SOdysseyAnimationTimelineCellNamesKey.h"
#include "OdysseyStyle.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyAnimationTimelineSection.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineCellNamesKey::Construct(const FArguments& InArgs)
{
    mCell = InArgs._Cell;
    check( mCell.IsSet() );

    ChildSlot
    [
        SNew(SOdysseyAnimationTimelineSection)
        .TimelinePosition(InArgs._TimelinePosition)
        .WidthInFrames(1)
        .HAlign(HAlign_Center)
        [
            SNew( SInlineEditableTextBlock ) //TODO: doesn't work, see SInlineEditableTextBlock::OnMouseButtonDown#267 ...
            .Text( this, &SOdysseyAnimationTimelineCellNamesKey::GetText )
            .IsEnabled_Lambda( [this](){ return mCell.Get()->GetLayer()->IsEditable();} )
            .OnTextCommitted (this, &SOdysseyAnimationTimelineCellNamesKey::OnNameCommited )
        ]
    ];
}

FText
SOdysseyAnimationTimelineCellNamesKey::GetText() const
{
    if( !IsValid( mCell.Get() ) )
        return LOCTEXT( "cell-names.timeline.no-cell", "*ERROR*" );

    if( mCell.Get()->HasNoName() )
        return FText::FromString( mCell.Get()->GetName( ECellNameIfEmpty::IndexInLayer ) + TEXT( "*" ) );

    return FText::FromString( mCell.Get()->GetName( ECellNameIfEmpty::None ) );
}

void
SOdysseyAnimationTimelineCellNamesKey::OnNameCommited( const FText& iText, ETextCommit::Type iType )
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "cell-names.transaction.set-name", "Change Cell Name" ) );

    mCell.Get()->SetName( iText.ToString() );
}

#undef LOCTEXT_NAMESPACE
