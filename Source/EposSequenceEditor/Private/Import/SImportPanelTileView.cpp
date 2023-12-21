// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/SImportPanelTileView.h"

#include "ImageUtils.h"
#include "Internationalization/BreakIterator.h"
#include "ISequencer.h"
#include "Slate/SlateTextures.h"
#include "Styling/StyleColors.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"
#include "Widgets/SViewport.h"

#include "Import/ImportImageSequenceSettings.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "ImageSequenceImportPanelTile"

//---

void
FImportPanelItem::CreateThumbnail()
{
    mThumbnail = FImageUtils::ImportFileAsTexture2D( mPanel->Pathfile.FilePath );

    if( !mThumbnail )
    {
        mBrush = new FSlateNoResource();
        return;
    }

    float ratio = mThumbnail->GetSizeX() / double( mThumbnail->GetSizeY() );

    mBrush = new FSlateImageBrush( mThumbnail, FVector2D( 256 * ratio, 256 ) );
}

//---

//static
TSharedRef<ITableRow>
SImportPanelTileView::BuildTile( TSharedPtr<FImportPanelItem> Item, const TSharedRef<STableViewBase>& OwnerTable, const TArray<TSharedPtr<FImportPanelItem>>* iListItemsSource )
{
    if( !ensure( Item.IsValid() ) )
    {
        return SNew( STableRow<TSharedPtr<FImportPanelItem>>, OwnerTable );
    }

    return SNew( SImportPanelTileView, OwnerTable, iListItemsSource ).Item( Item );
}

void
SImportPanelTileView::Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable, const TArray<TSharedPtr<FImportPanelItem>>* iListItemsSource )
{
    mListItemsSource = iListItemsSource;
    check( mListItemsSource );

    check( InArgs._Item.IsValid() );
    mPanelItem = InArgs._Item;

    //---

    STableRow::Construct(
        STableRow::FArguments()
        .Style( FEposSequenceEditorStyle::Get(), "ImportImageSequence.TableRow" )
        .Padding( this, &SImportPanelTileView::GetShotPadding )
        .Content()
        [
            SNew( SBorder )
            .Padding( FMargin( 0.f, 0.f, 1.f, 1.f ) )
            .BorderImage( FEposSequenceEditorStyle::Get().GetBrush( "ImportImageSequence.PanelItem.DropShadow" ) )
            .ToolTipText( this, &SImportPanelTileView::GetTooltipText )
            [
                SNew( SVerticalBox )

                // Top info
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Fill )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SImportPanelTileView::GetTopAreaBackgroundBrush )
                    [
                        SNew( SVerticalBox )

                        // Board Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SBorder )
                            .Padding( 3.f )
                            .HAlign( HAlign_Fill )
                            .BorderImage( this, &SImportPanelTileView::GetBoardAreaBackgroundBrush )
                            [
                                SNew( STextBlock )
                                .Text( FText::Format( LOCTEXT( "item-label.board-id", "Board: {0}" ), FText::FromString( mPanelItem->mBoard->Id ) ) )
                                .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                            ]
                        ]

                        // Shot Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SBorder )
                            .Padding( 3.f )
                            .HAlign( HAlign_Fill )
                            .BorderImage( this, &SImportPanelTileView::GetShotAreaBackgroundBrush )
                            [
                                SNew( STextBlock )
                                .Text( FText::Format( LOCTEXT( "item-label.shot-id", "Shot: {0}" ), FText::FromString( mPanelItem->mShot->Id ) ) )
                                .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                            ]
                        ]

                        // Panel Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( SBorder )
                            .Padding( 3.f )
                            .HAlign( HAlign_Fill )
                            .BorderImage( this, &SImportPanelTileView::GetPanelAreaBackgroundBrush )
                            [
                                SNew( STextBlock )
                                //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                                .Text( FText::Format( LOCTEXT( "item-label.panel-id", "Panel: {0}" ), FText::FromString( mPanelItem->mPanel->Id ) ) )
                                .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                            ]
                        ]
                    ]
                ]

                // Thumbnail
                + SVerticalBox::Slot()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Center )
                    .VAlign( VAlign_Center )
                    .Padding( FMargin( 0 ) )
                    .BorderImage( this, &SImportPanelTileView::GetThumbnailAreaBackgroundBrush )
                    [
                        SNew( SScaleBox )
                        .Stretch( EStretch::ScaleToFit )
                        [
                            SNew( SImage )
                            .Image( mPanelItem->mBrush )
                        ]
                    ]
                ]

                // Bottom Info
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Fill )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SImportPanelTileView::GetBottomAreaBackgroundBrush )
                    [
                        SNew( SHorizontalBox )

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        [
                            SNew( STextBlock )
                            .Text( LOCTEXT( "item-label.panel-duration", "Duration: " ) )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                        ]

                        + SHorizontalBox::Slot()
                        [
                            SNew( SInlineEditableTextBlock )
                            .Text( this, &SImportPanelTileView::GetDurationText )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                            .OnVerifyTextChanged( this, &SImportPanelTileView::OnDurationTextVerify )
                            .OnTextCommitted( this, &SImportPanelTileView::OnDurationTextCommitted )
                        ]
                    ]
                ]
            ]
        ]
        , OwnerTable );
}

//---

FText
SImportPanelTileView::GetTooltipText() const
{
    TArray<FText> tooltip_texts;

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.panel-pathfile", "Pathfile: {0}" ), FText::FromString( mPanelItem->mPanel->Pathfile.FilePath ) );
        tooltip_texts.Add( line );
    }

    tooltip_texts.Add( FText::GetEmpty() );

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.board-id", "Board: {0}" ), FText::FromString( mPanelItem->mBoard->Id ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.shot-id", "Shot: {0}" ), FText::FromString( mPanelItem->mShot->Id ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.panel-id", "Panel: {0}" ), FText::FromString( mPanelItem->mPanel->Id ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.panel-duration", "Panel Duration: {0}" ), FText::AsNumber( mPanelItem->mPanel->Duration ) );
        tooltip_texts.Add( line );
    }

    return FText::Join( FText::FromString( TEXT( "\n" ) ), tooltip_texts );
}

FText
SImportPanelTileView::GetDurationText() const
{
    return FText::AsNumber( mPanelItem->mPanel->Duration );
}

void
SImportPanelTileView::OnDurationTextCommitted( const FText& iText, ETextCommit::Type iType )
{
    mPanelItem->mPanel->Duration = FCString::Atoi( *iText.ToString() );
}

bool
SImportPanelTileView::OnDurationTextVerify( const FText& iText, FText& iError )
{
    if( !iText.IsNumeric() )
        return false;

    return true;
}

//---

bool
SImportPanelTileView::IsBoardEven() const
{
    int32 index = INDEX_NONE;
    for( int i = 0; i < mPanelItem->mRootStruct->Boards.Num(); i++ )
    {
        if( mPanelItem->mBoard == &mPanelItem->mRootStruct->Boards[i] )
            return !!( i % 2 );
    }

    return true;
}

bool
SImportPanelTileView::IsShotEven() const
{
    int32 index = INDEX_NONE;
    for( int i = 0; i < mPanelItem->mBoard->Shots.Num(); i++ )
    {
        if( mPanelItem->mShot == &mPanelItem->mBoard->Shots[i] )
            return !!( i % 2 );
    }

    return true;
}

FMargin
SImportPanelTileView::GetBoardPadding() const
{
    const TArray<TSharedPtr<FImportPanelItem>>& items = *mListItemsSource;

    FMargin margin( 0.f, 2.f );

    int32 current_index = IndexInList;
    //int32 previous_index = IndexInList - 1;
    int32 next_index = IndexInList + 1;

    if( items.IsValidIndex( next_index ) )
    {
        if( items[next_index]->mBoard->Id != mPanelItem->mBoard->Id )
            margin.Right = 10.f;
    }

    return margin;
}

FMargin
SImportPanelTileView::GetShotPadding() const
{
    const TArray<TSharedPtr<FImportPanelItem>>& items = *mListItemsSource;

    FMargin margin( 0.f, 2.f );

    int32 current_index = IndexInList;
    //int32 previous_index = IndexInList - 1;
    int32 next_index = IndexInList + 1;

    if( items.IsValidIndex( next_index ) )
    {
        if( items[next_index]->mShot->Id != mPanelItem->mShot->Id )
            margin.Right = 10.f;
    }

    return margin;
}

//---

const FSlateBrush*
SImportPanelTileView::GetBoardAreaBackgroundBrush() const
{
    static const FName even( "ImportImageSequence.PanelItem.BoardAreaBackground.Even" );
    static const FName odd( "ImportImageSequence.PanelItem.BoardAreaBackground.Odd" );

    return IsBoardEven()
        ?
        FEposSequenceEditorStyle::Get().GetBrush( even )
        :
        FEposSequenceEditorStyle::Get().GetBrush( odd );
}

const FSlateBrush*
SImportPanelTileView::GetShotAreaBackgroundBrush() const
{
    static const FName even( "ImportImageSequence.PanelItem.ShotAreaBackground.Even" );
    static const FName odd( "ImportImageSequence.PanelItem.ShotAreaBackground.Odd" );

    return IsShotEven()
        ?
        FEposSequenceEditorStyle::Get().GetBrush( even )
        :
        FEposSequenceEditorStyle::Get().GetBrush( odd );
}

const FSlateBrush*
SImportPanelTileView::GetPanelAreaBackgroundBrush() const
{
    static const FName brush( "ImportImageSequence.PanelItem.PanelAreaBackground" );

    return FEposSequenceEditorStyle::Get().GetBrush( brush );
}

//---

const FSlateBrush*
SImportPanelTileView::GetTopAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    static const FName even( "ImportImageSequence.PanelItem.TopAreaBackground.Even" );
    static const FName odd( "ImportImageSequence.PanelItem.TopAreaBackground.Odd" );

    static const FName even_hover( "ImportImageSequence.PanelItem.TopAreaBackground.Even.Hover" );
    static const FName odd_hover( "ImportImageSequence.PanelItem.TopAreaBackground.Odd.Hover" );

    if( IsBoardEven() )
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( even_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( even );
    }
    else
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( odd_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( odd );
    }
}

const FSlateBrush*
SImportPanelTileView::GetThumbnailAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    static const FName even( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Even" );
    static const FName odd( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Odd" );

    static const FName even_hover( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Even.Hover" );
    static const FName odd_hover( "ImportImageSequence.PanelItem.ThumbnailAreaBackground.Odd.Hover" );

    if( IsBoardEven() )
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( even_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( even );
    }
    else
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( odd_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( odd );
    }
}

const FSlateBrush*
SImportPanelTileView::GetBottomAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    static const FName even( "ImportImageSequence.PanelItem.BottomAreaBackground.Even" );
    static const FName odd( "ImportImageSequence.PanelItem.BottomAreaBackground.Odd" );

    static const FName even_hover( "ImportImageSequence.PanelItem.BottomAreaBackground.Even.Hover" );
    static const FName odd_hover( "ImportImageSequence.PanelItem.BottomAreaBackground.Odd.Hover" );

    if( IsBoardEven() )
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( even_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( even );
    }
    else
    {
        if( bIsRowHovered )
            return FEposSequenceEditorStyle::Get().GetBrush( odd_hover );
        else
            return FEposSequenceEditorStyle::Get().GetBrush( odd );
    }
}

FSlateColor
SImportPanelTileView::GetNameAreaTextColor() const
{
    //const bool bIsSelected = IsSelected();
    //const bool bIsRowHovered = IsHovered();

    //if( bIsSelected || bIsRowHovered )
    //{
    //    return FStyleColors::White;
    //}

    return FSlateColor::UseForeground();
}

#undef LOCTEXT_NAMESPACE
