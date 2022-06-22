// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/SExportPanelTileView.h"

#include "Internationalization/BreakIterator.h"
#include "ISequencer.h"
#include "Slate/SlateTextures.h"
#include "Styling/StyleColors.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SViewport.h"

#include "Export/ExportImageSequenceNamingFormatter.h"
#include "Export/ExportImageSequenceSettings.h"
#include "Export/PanelThumbnail.h"
#include "Export/SceneRenderer.h"
#include "Styles/EposSequenceEditorStyle.h"

#define LOCTEXT_NAMESPACE "ImageSequenceExportPanelTile"

//---

void
FPanelItem::CreateThumbnail()
{
    check( mSequencer.IsValid() );

    double ratio = mOptions->ImageSize.X / double( mOptions->ImageSize.Y );

    mThumbnail = MakeShareable( new FPanelThumbnail() );
    mThumbnail->ResizeRenderTarget( FIntPoint( 256 * ratio, 256 ) );

    FSceneRenderer thumbnail_renderer( mSequencer, &mPanel, mOptions );
    thumbnail_renderer.OverrideSize( mThumbnail->GetSize() );
    thumbnail_renderer.RenderPlane( mThumbnail->GetRenderTarget() );
}

//---

//static
TSharedRef<ITableRow>
SPanelTileView::BuildTile( TSharedPtr<FPanelItem> Item, const TSharedRef<STableViewBase>& OwnerTable )
{
    if( !ensure( Item.IsValid() ) )
    {
        return SNew( STableRow<TSharedPtr<FPanelItem>>, OwnerTable );
    }

    return SNew( SPanelTileView, OwnerTable ).Item( Item );
}

void
SPanelTileView::Construct( const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTable )
{
    check( InArgs._Item.IsValid() );
    mPanelItem = InArgs._Item;

    FString panel_global_frame = mPanelItem->mSequencer.Pin()->GetNumericTypeInterface()->ToString( mPanelItem->mPanel.GlobalFrame.Value );

    //---

    STableRow::Construct(
        STableRow::FArguments()
        .Style( FEposSequenceEditorStyle::Get(), "ExportImageSequence.TableRow" )
        .Padding( 2.0f )
        .Content()
        [
            SNew( SBorder )
            .Padding( FMargin( 0.0f, 0.0f, 5.0f, 5.0f ) )
            .BorderImage( FEposSequenceEditorStyle::Get().GetBrush( "ExportImageSequence.PanelItem.DropShadow" ) )
            .ToolTipText( this, &SPanelTileView::GetTooltipText )
            [
                SNew( SVerticalBox )

                // Frame
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Center )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SPanelTileView::GetTopAreaBackgroundBrush )
                    [
                        SNew( STextBlock )
                        //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                        .Text( FText::FromString( panel_global_frame ) )
                        .ColorAndOpacity( this, &SPanelTileView::GetNameAreaTextColor )
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
                            SNew( SViewport )
                            .ViewportSize( mPanelItem->mThumbnail->GetSize() )
                            .EnableGammaCorrection( false )
                            .IgnoreTextureAlpha( false )
                            .EnableBlending( true )
                            .PreMultipliedAlpha( false )
                            .ViewportInterface( mPanelItem->mThumbnail )
                        ]
                    ]
                ]

                // Export
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Center )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SPanelTileView::GetBottomAreaBackgroundBrush )
                    [
                        SNew( SCheckBox )
                        .IsChecked( this, &SPanelTileView::GetExportCheckBoxState )
                        .OnCheckStateChanged( this, &SPanelTileView::HandleExportCheckStateChanged )
                        .ToolTipText( LOCTEXT( "export-panel.tooltip", "Export this panel" ) )
                    ]
                ]
            ]
        ]
        , OwnerTable );
}

//---

void
SPanelTileView::HandleExportCheckStateChanged( const ECheckBoxState iNewCheckedState )
{
    mPanelItem->mExport = ( iNewCheckedState == ECheckBoxState::Checked ) ? true : false;
}

ECheckBoxState
SPanelTileView::GetExportCheckBoxState() const
{
    return mPanelItem->mExport ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

FText
SPanelTileView::GetTooltipText() const
{
    TArray<FText> tooltip_texts;

    FExportImageSequenceNamingFormatter name_formatter( mPanelItem->mSequencer, &mPanelItem->mPanel, mPanelItem->mIndex, mPanelItem->mOptions );
    FString export_name;
    bool formatting = name_formatter.FormatName( export_name );
    if( formatting )
    {
        FText line = FText::Format( LOCTEXT( "item.name.tooltip", "Filename: {0}" ), FText::FromString( export_name ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item.size.tooltip", "Size: {0}x{1}" ), FText::AsNumber( mPanelItem->mOptions->ImageSize.X ), FText::AsNumber( mPanelItem->mOptions->ImageSize.Y ) );
        tooltip_texts.Add( line );
    }

    return FText::Join( FText::FromString( TEXT( "\n" ) ), tooltip_texts );
}

//---

const FSlateBrush*
SPanelTileView::GetTopAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    if( bIsRowHovered )
    {
        static const FName Hovered( "ExportImageSequence.PanelItem.TopAreaHoverBackground" );
        return FEposSequenceEditorStyle::Get().GetBrush( Hovered );
    }

    return FEposSequenceEditorStyle::Get().GetBrush( "ExportImageSequence.PanelItem.TopAreaBackground" );
}

const FSlateBrush*
SPanelTileView::GetBottomAreaBackgroundBrush() const
{
    const bool bIsRowHovered = IsHovered();

    if( bIsRowHovered )
    {
        static const FName Hovered( "ExportImageSequence.PanelItem.BottomAreaHoverBackground" );
        return FEposSequenceEditorStyle::Get().GetBrush( Hovered );
    }

    return FEposSequenceEditorStyle::Get().GetBrush( "ExportImageSequence.PanelItem.BottomAreaBackground" );
}

FSlateColor
SPanelTileView::GetNameAreaTextColor() const
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
