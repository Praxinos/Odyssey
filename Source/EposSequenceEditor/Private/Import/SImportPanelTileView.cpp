// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Import/SImportPanelTileView.h"

#include "ImageUtils.h"
#include "Internationalization/BreakIterator.h"
#include "ISequencer.h"
#include "Slate/SlateTextures.h"
#include "Styling/StyleColors.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SViewport.h"

#include "Import/ImportImageSequenceSettings.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "ImageSequenceImportPanelTile"

//---

void
FImportPanelItem::CreateThumbnail()
{
    mThumbnail = FImageUtils::ImportFileAsTexture2D( mFrame.Pathfile.FilePath );

    float ratio = mThumbnail->GetSizeX() / double( mThumbnail->GetSizeY() );

    mBrush = new FSlateImageBrush( mThumbnail, FVector2D( 256 * ratio, 256 ) );
}

//---

//static
TSharedRef<ITableRow>
SImportPanelTileView::BuildTile( TSharedPtr<FImportPanelItem> Item, const TSharedRef<STableViewBase>& OwnerTable )
{
    if( !ensure( Item.IsValid() ) )
    {
        return SNew( STableRow<TSharedPtr<FImportPanelItem>>, OwnerTable );
    }

    return SNew( SImportPanelTileView, OwnerTable ).Item( Item );
}

void
SImportPanelTileView::Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable )
{
    check( InArgs._Item.IsValid() );
    mPanelItem = InArgs._Item;

    //---

    STableRow::Construct(
        STableRow::FArguments()
        .Style( FEposSequenceEditorStyle::Get(), "ImportImageSequence.TableRow" )
        //.Padding( 0.0f )
        .Padding( 2.0f )
        .Content()
        [
            SNew( SBorder )
            //.Padding( 0.0f )
            .Padding( FMargin( 0.0f, 0.0f, 5.0f, 5.0f ) )
            .BorderImage( FEposSequenceEditorStyle::Get().GetBrush( "ImportImageSequence.PanelItem.DropShadow" ) )
            .ToolTipText( this, &SImportPanelTileView::GetTooltipText )
            [
                SNew( SVerticalBox )

                // Top info
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Center )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SImportPanelTileView::GetTopAreaBackgroundBrush )
                    [
                        SNew( SVerticalBox )

                        // Board Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( STextBlock )
                            .Text( FText::Format( LOCTEXT( "item-label.board-id", "Sequence: {0}" ), FText::FromString( mPanelItem->mBoardId ) ) )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                        ]

                        // Shot Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( STextBlock )
                            .Text( FText::Format( LOCTEXT( "item-label.shot-id", "Shot: {0}" ), FText::FromString( mPanelItem->mShotId ) ) )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                        ]

                        // Frame Id
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( STextBlock )
                            //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                            .Text( FText::Format( LOCTEXT( "item-label.frame-id", "Panel: {0}" ), FText::FromString( mPanelItem->mFrame.Id ) ) )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
                        ]

                        // Frame Duration
                        + SVerticalBox::Slot()
                        .AutoHeight()
                        [
                            SNew( STextBlock )
                            .Text( FText::Format( LOCTEXT( "item-label.frame-duration", "Duration: {0}" ), FText::AsNumber( mPanelItem->mFrame.Duration ) ) )
                            .ColorAndOpacity( this, &SImportPanelTileView::GetNameAreaTextColor )
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
                    .BorderImage( FEposSequenceEditorStyle::Get().GetBrush( "ExportImageSequence.PanelItem.ThumbnailAreaBackground" ) )
                    [
                        SNew( SScaleBox )
                        .Stretch( EStretch::ScaleToFit )
                        [
                            SNew( SImage )
                            .Image( mPanelItem->mBrush )
                        ]
                    ]
                ]

                //// Export
                //+ SVerticalBox::Slot()
                //.AutoHeight()
                //[
                //    SNew( SBorder )
                //    .HAlign( HAlign_Center )
                //    .Padding( FMargin( 3.0f, 3.0f ) )
                //    .BorderImage( this, &SImportPanelTileView::GetBottomAreaBackgroundBrush )
                //    [
                //        SNew( SCheckBox )
                //        .IsChecked( this, &SImportPanelTileView::GetExportCheckBoxState )
                //        .OnCheckStateChanged( this, &SImportPanelTileView::HandleExportCheckStateChanged )
                //        .ToolTipText( LOCTEXT( "export-panel.tooltip", "Export this panel" ) )
                //    ]
                //]
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
        FText line = FText::Format( LOCTEXT( "item-tooltip.frame-pathfile", "Pathfile: {0}" ), FText::FromString( mPanelItem->mFrame.Pathfile.FilePath ) );
        tooltip_texts.Add( line );
    }

    tooltip_texts.Add( FText::GetEmpty() );

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.board-id", "Sequence: {0}" ), FText::FromString( mPanelItem->mBoardId ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.shot-id", "Shot: {0}" ), FText::FromString( mPanelItem->mShotId ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.frame-id", "Panel: {0}" ), FText::FromString( mPanelItem->mFrame.Id ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item-tooltip.frame-duration", "Panel Duration: {0}" ), FText::AsNumber( mPanelItem->mFrame.Duration ) );
        tooltip_texts.Add( line );
    }

    return FText::Join( FText::FromString( TEXT( "\n" ) ), tooltip_texts );
}

//---

const FSlateBrush*
SImportPanelTileView::GetTopAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    if( bIsRowHovered )
    {
        static const FName Hovered( "ImportImageSequence.PanelItem.TopAreaHoverBackground" );
        return FEposSequenceEditorStyle::Get().GetBrush( Hovered );
    }

    return FEposSequenceEditorStyle::Get().GetBrush( "ImportImageSequence.PanelItem.TopAreaBackground" );
}

const FSlateBrush*
SImportPanelTileView::GetBottomAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    if( bIsRowHovered )
    {
        static const FName Hovered( "ImportImageSequence.PanelItem.BottomAreaHoverBackground" );
        return FEposSequenceEditorStyle::Get().GetBrush( Hovered );
    }

    return FEposSequenceEditorStyle::Get().GetBrush( "ImportImageSequence.PanelItem.BottomAreaBackground" );
}

FSlateColor
SImportPanelTileView::GetNameAreaTextColor() const
{
    const bool bIsSelected = IsSelected();
    const bool bIsRowHovered = IsHovered();

    if( bIsSelected || bIsRowHovered )
    {
        return FStyleColors::White;
    }

    return FSlateColor::UseForeground();
}

#undef LOCTEXT_NAMESPACE
