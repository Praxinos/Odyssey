// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageRaster.h"

#include "ImageUtils.h"
#include "ObjectTools.h"

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorSettings.h"

#define THUMBNAIL_SIZE 32

namespace
{
    static TStrongObjectPtr<UTexture2D> sCheckerboardTexture;
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

    RenderTarget->InitAutoFormat( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE ), THUMBNAIL_SIZE );
    //RenderTarget->ResizeTarget( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE ), THUMBNAIL_SIZE );
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
    ThumbnailTools::RenderThumbnail( mCell, mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE ), THUMBNAIL_SIZE, ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, RTResource );
}

void
SOdysseyAnimationCellImageRaster::CreateCheckerboardTexture()
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    if( !sCheckerboardTexture
        || sRasterCheckerboardColorOne != settings.GetCheckerColorOne()
        || sRasterCheckerboardColorTwo != settings.GetCheckerColorTwo()
        )
    {
        sCheckerboardTexture = TStrongObjectPtr<UTexture2D>( FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), 16 ) );
        sRasterCheckerboardColorOne = settings.GetCheckerColorOne();
        sRasterCheckerboardColorTwo = settings.GetCheckerColorTwo();
    }

    //---

    if( !mCheckerboardBrush )
        mCheckerboardBrush = new FSlateImageBrush( sCheckerboardTexture.Get(), FVector2D( sCheckerboardTexture->GetSurfaceWidth(), sCheckerboardTexture->GetSurfaceHeight() ), FSlateColor( FLinearColor::White ), ESlateBrushTileType::Both );

    mCheckerboardBrush->SetResourceObject( sCheckerboardTexture.Get() );
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
