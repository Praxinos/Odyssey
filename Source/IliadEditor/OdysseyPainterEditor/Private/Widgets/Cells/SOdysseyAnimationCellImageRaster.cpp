// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageRaster.h"

#include "Brushes/SlateImageBrush.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorSettings.h"

#define THUMBNAIL_SIZE_RASTER 32

namespace
{
    static TStrongObjectPtr<UTexture2D> sRasterCheckerboardTexture;
    static FColor sRasterCheckerboardColorOne = FColor( EForceInit::ForceInit );
    static FColor sRasterCheckerboardColorTwo = FColor( EForceInit::ForceInit );
};

SOdysseyAnimationCellImageRaster::~SOdysseyAnimationCellImageRaster()
{
    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().RemoveAll( this );
    settings->GetOnCheckerSizeChanged().RemoveAll( this );

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

void
SOdysseyAnimationCellImageRaster::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageRaster* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationCellImageRaster::OnRenderingChanged);

    //---

    RenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>( NewObject<UTextureRenderTarget2D>() );
    RenderTarget->RenderTargetFormat = RTF_RGBA8;

    RenderTarget->InitAutoFormat( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_RASTER ), THUMBNAIL_SIZE_RASTER );
    //RenderTarget->ResizeTarget( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_RASTER ), THUMBNAIL_SIZE_RASTER );
    //RenderTarget->UpdateResource();
    RenderTarget->UpdateResourceImmediate();

    mCellBrush = new FSlateImageBrush( RenderTarget.Get(), FVector2D( RenderTarget->GetSurfaceWidth(), RenderTarget->GetSurfaceHeight() ) );

    RefreshCellRenderTarget();

    //---

    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().AddSP( this, &SOdysseyAnimationCellImageRaster::CreateCheckerboardTexture );
    settings->GetOnCheckerSizeChanged().AddSP( this, &SOdysseyAnimationCellImageRaster::CreateCheckerboardTexture );

    CreateCheckerboardTexture();

    //---

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    [
        SNew(SHorizontalBox)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
        .Visibility(this, &SOdysseyAnimationCellImageRaster::GetContentVisibility)
        + SHorizontalBox::Slot()
        .AutoWidth()
        [
            SNew( SOverlay )
            + SOverlay::Slot()
            [
                SNew( SImage )
                .Image( mCheckerboardBrush )
            ]
            + SOverlay::Slot()
            [
                SNew( SImage )
                .Image( mCellBrush )
            ]
        ]
    ];
}

void
SOdysseyAnimationCellImageRaster::RefreshCellRenderTarget()
{
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
    ThumbnailTools::RenderThumbnail( mCell, mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_RASTER ), THUMBNAIL_SIZE_RASTER, ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, RTResource );
}

void
SOdysseyAnimationCellImageRaster::CreateCheckerboardTexture()
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    if( !sRasterCheckerboardTexture
        || sRasterCheckerboardColorOne != settings.GetCheckerColorOne()
        || sRasterCheckerboardColorTwo != settings.GetCheckerColorTwo()
        )
    {
        sRasterCheckerboardTexture = TStrongObjectPtr<UTexture2D>( FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), 16 ) );
        sRasterCheckerboardColorOne = settings.GetCheckerColorOne();
        sRasterCheckerboardColorTwo = settings.GetCheckerColorTwo();
    }

    //---

    if( !mCheckerboardBrush )
        mCheckerboardBrush = new FSlateImageBrush( sRasterCheckerboardTexture.Get(), FVector2D( sRasterCheckerboardTexture->GetSurfaceWidth(), sRasterCheckerboardTexture->GetSurfaceHeight() ), FSlateColor( FLinearColor::White ), ESlateBrushTileType::Both );

    mCheckerboardBrush->SetResourceObject( sRasterCheckerboardTexture.Get() );
}

void
SOdysseyAnimationCellImageRaster::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    if (!mCell)
        return;

    if (iEvent.IsInteractive())
        return;

    TArray<FGuid> composition = mCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0);
    if( composition.Contains( iEvent.GetId() ) )
        RefreshCellRenderTarget();
}

EVisibility
SOdysseyAnimationCellImageRaster::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
