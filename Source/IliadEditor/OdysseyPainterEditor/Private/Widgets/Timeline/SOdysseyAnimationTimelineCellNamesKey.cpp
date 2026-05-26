// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationTimelineCellNamesKey.h"

#include "ScopedTransaction.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "OdysseyStyle.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyAnimationTimelineSection.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

void
SOdysseyAnimationTimelineCellNamesKey::Construct(const FArguments& InArgs)
{
    mCell = InArgs._Cell;
    check( mCell.IsSet() );

    ChildSlot
    [
        SNew( SInlineEditableTextBlock ) //TODO: doesn't work, see SInlineEditableTextBlock::OnMouseButtonDown#267 ...
        .Text( this, &SOdysseyAnimationTimelineCellNamesKey::GetText )
        .ToolTipText( LOCTEXT( "cell-name.timeline.name.tooltip", "The cell name" ) )
        .Font( this, &SOdysseyAnimationTimelineCellNamesKey::GetFontInfo )
        .ColorAndOpacity( this, &SOdysseyAnimationTimelineCellNamesKey::GetColor )
        .IsEnabled_Lambda( [this](){ return mCell.Get()->GetLayer()->IsEditable();} )
        .OnTextCommitted( this, &SOdysseyAnimationTimelineCellNamesKey::OnNameCommited )
    ];
}

FText
SOdysseyAnimationTimelineCellNamesKey::GetText() const
{
    if( !IsValid( mCell.Get() ) )
        return LOCTEXT( "cell-name.timeline.no-cell", "*ERROR*" );

    if( mCell.Get()->HasNoName() )
        return FText::FromString( mCell.Get()->GetName( ECellNameIfEmpty::IndexInLayer ) );

    return FText::FromString( mCell.Get()->GetName( ECellNameIfEmpty::None ) );
}

FSlateFontInfo
SOdysseyAnimationTimelineCellNamesKey::GetFontInfo() const
{
    if( !IsValid( mCell.Get() ) )
        return FCoreStyle::Get().GetFontStyle( "NormalFont" );

    if( mCell.Get()->HasNoName() )
        return FCoreStyle::Get().GetFontStyle( "NormalFontItalic" );

    //TODO: maybe check if we are in editing mode as .Font(...) of the widget is shared between the textblock AND the textbox
    // to not have italic in textbox ?
    return FCoreStyle::Get().GetFontStyle( "NormalFont" );
}

FSlateColor
SOdysseyAnimationTimelineCellNamesKey::GetColor() const
{
    if( !IsValid( mCell.Get() ) )
        return FSlateColor::UseSubduedForeground();

    if( mCell.Get()->HasNoName() )
        return FSlateColor::UseSubduedForeground();

    return FSlateColor::UseForeground();
}

void
SOdysseyAnimationTimelineCellNamesKey::OnNameCommited( const FText& iText, ETextCommit::Type iType )
{
    FScopedTransaction ScopedTransaction( LOCTEXT( "cell-name.transaction.set-name", "Change Cell Name" ) );

    mCell.Get()->SetName( iText.ToString() );
}

#undef LOCTEXT_NAMESPACE
