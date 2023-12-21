// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ImageSequence/SExportPanelTileView.h"

#include "Internationalization/BreakIterator.h"
#include "ISequencer.h"
#include "MovieSceneSequence.h"
#include "Slate/SlateTextures.h"
#include "Styling/StyleColors.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/SViewport.h"

#include "Export/ImageSequence/ExportImageSequenceNamingFormatter.h"
#include "Export/ImageSequence/ExportImageSequenceSettings.h"
#include "Export/ImageSequence/PanelThumbnail.h"
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

    FSceneRenderer thumbnail_renderer( mSequencer, &mPanel, mThumbnail->GetSize() );
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

    FText panel_global_frame = FText::Format( LOCTEXT( "panel-item.global-frame", "{0}" ), FText::FromString( mPanelItem->mSequencer.Pin()->GetNumericTypeInterface()->ToString( mPanelItem->mPanel.GlobalFrame.Value ) ) );

    FText panel_sequence = mPanelItem->mPanel.mSequence->GetDisplayName();

    FText panel_source;
    if( mPanelItem->mPanel.mSourceMark.IsSet() )
    {
        const FExportPanelSourceMark& source_mark = mPanelItem->mPanel.mSourceMark.GetValue();

        panel_source = FText::Format( LOCTEXT( "panel-item.source.mark", "Mark: {0}" ), FText::FromString( source_mark.mMark.Label ) );
    }

    if( mPanelItem->mPanel.mSourceDrawing.IsSet() )
    {
        const FExportPanelSourceDrawing& source_drawing = mPanelItem->mPanel.mSourceDrawing.GetValue();

        if( source_drawing.mDrawings.Num() == 1 )
        {
            //const FDrawing& drawing = source_drawing.mDrawings[0].mDrawing;
            FGuid binding = source_drawing.mDrawings[0].mBindingId;

            //FMovieSceneObjectPathChannel* channel = drawing.mChannel;
            //int32 index = channel->GetData().GetIndex( drawing.mKeyHandle );

            FText track_name = mPanelItem->mPanel.mSequence->GetMovieScene()->GetObjectDisplayName( binding );

            panel_source = FText::Format( LOCTEXT( "panel-item.source.drawing-1", "{0}" ), track_name );
        }
        else if( source_drawing.mDrawings.Num() > 1 )
        {
            panel_source = LOCTEXT( "panel-item.source.drawing-n", "multiple planes" );
        }
    }

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
                    .HAlign( HAlign_Fill )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SPanelTileView::GetTopAreaBackgroundBrush )
                    [
                        SNew( SVerticalBox )

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                            .Text( panel_global_frame )
                            .ColorAndOpacity( this, &SPanelTileView::GetNameAreaTextColor )
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                            .Text( panel_sequence )
                            .ColorAndOpacity( this, &SPanelTileView::GetNameAreaTextColor )
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
                            SNew( SViewport )
                            .ViewportSize( mPanelItem->mThumbnail->GetSize() )
                            .EnableGammaCorrection( false )
                            .IgnoreTextureAlpha( false )
                            .EnableBlending( true )
                            .PreMultipliedAlpha( false )
                            .IgnoreTextureAlpha( true )
                            .ViewportInterface( mPanelItem->mThumbnail )
                        ]
                    ]
                ]

                // Export
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SBorder )
                    .HAlign( HAlign_Fill )
                    .Padding( FMargin( 3.0f, 3.0f ) )
                    .BorderImage( this, &SPanelTileView::GetBottomAreaBackgroundBrush )
                    [
                        SNew( SVerticalBox )

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign( HAlign_Center )
                        [
                            SNew( STextBlock )
                            //.Font( FEposSequenceEditorStyle::Get().GetFontStyle( "ExportImageSequence.PanelItem.Font" ) )
                            .Text( panel_source )
                            .ColorAndOpacity( this, &SPanelTileView::GetNameAreaTextColor )
                        ]

                        + SVerticalBox::Slot()
                        .AutoHeight()
                        .HAlign( HAlign_Center )
                        [
                            SNew( SCheckBox )
                            .IsChecked( this, &SPanelTileView::GetExportCheckBoxState )
                            .OnCheckStateChanged( this, &SPanelTileView::HandleExportCheckStateChanged )
                            .ToolTipText( LOCTEXT( "export-panel.tooltip", "Export this panel" ) )
                        ]
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
    bool is_formatted = name_formatter.FormatName( mPanelItem->mOptions->Pattern, export_name );
    if( is_formatted )
    {
        FText line = FText::Format( LOCTEXT( "item.name.tooltip", "Filename: {0}" ), FText::FromString( export_name ) );
        tooltip_texts.Add( line );
    }

    {
        FText line = FText::Format( LOCTEXT( "item.size.tooltip", "Size: {0}x{1}" ), FText::AsNumber( mPanelItem->mOptions->ImageSize.X ), FText::AsNumber( mPanelItem->mOptions->ImageSize.Y ) );
        tooltip_texts.Add( line );
    }

    tooltip_texts.Add( FText::GetEmpty() );

    {
        FText line = FText::Format( LOCTEXT( "item.source-sequence.tooltip", "Shot: {0}" ), mPanelItem->mPanel.mSequence->GetDisplayName() );
        tooltip_texts.Add( line );
    }

    tooltip_texts.Add( FText::GetEmpty() );

    if( mPanelItem->mPanel.mSourceMark.IsSet() )
    {
        const FExportPanelSourceMark& source_mark = mPanelItem->mPanel.mSourceMark.GetValue();

        FText line = FText::Format( LOCTEXT( "item.source-mark.tooltip", "Mark: {0}" ), FText::FromString( source_mark.mMark.Label ) );
        tooltip_texts.Add( line );
    }

    if( mPanelItem->mPanel.mSourceDrawing.IsSet() && mPanelItem->mPanel.mSourceDrawing.GetValue().mDrawings.Num() )
    {
        FText line = FText::Format( LOCTEXT( "item.source-drawing-list.tooltip", "Drawing appearing in {0}|plural(one=plane,other=planes):" ), mPanelItem->mPanel.mSourceDrawing.GetValue().mDrawings.Num() );
        tooltip_texts.Add( line );

        const FExportPanelSourceDrawing& source_drawing = mPanelItem->mPanel.mSourceDrawing.GetValue();

        for( auto drawing_and_binding : source_drawing.mDrawings )
        {
            //const FDrawing& drawing = drawing_and_binding.mDrawing;
            FGuid binding = drawing_and_binding.mBindingId;

            //FMovieSceneObjectPathChannel* channel = drawing.mChannel;
            //int32 index = channel->GetData().GetIndex( drawing.mKeyHandle );

            line = FText::Format( LOCTEXT( "item.source-drawing-entry.tooltip", "- {0}" ), mPanelItem->mPanel.mSequence->GetMovieScene()->GetObjectDisplayName( binding ) );
            tooltip_texts.Add( line );
        }
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
