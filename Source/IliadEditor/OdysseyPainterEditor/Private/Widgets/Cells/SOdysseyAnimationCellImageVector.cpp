// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageVector.h"

#include "ImageUtils.h"
#include "ObjectTools.h"
#include "ThumbnailRendering/ThumbnailManager.h"

#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorSettings.h"

#define THUMBNAIL_SIZE 32

namespace
{
    static TStrongObjectPtr<UTexture2D> sCheckerboardTexture;
    static FColor sRasterCheckerboardColorOne = FColor( EForceInit::ForceInit );
    static FColor sRasterCheckerboardColorTwo = FColor( EForceInit::ForceInit );
};

SOdysseyAnimationCellImageVector::~SOdysseyAnimationCellImageVector()
{
    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().RemoveAll( this );
    settings->GetOnCheckerSizeChanged().RemoveAll( this );

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll(this);
}

void
SOdysseyAnimationCellImageVector::Construct(const FArguments& iArgs, UOdysseyAnimationCellImageVector* iCell)
{
    mShowContent = iArgs._ShowContent;

    if (!iCell)
        return;

    mCell = iCell;
    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP(this, &SOdysseyAnimationCellImageVector::OnRenderingChanged);

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
    settings->GetOnCheckerColorChanged().AddSP( this, &SOdysseyAnimationCellImageVector::CreateCheckerboardTexture );
    settings->GetOnCheckerSizeChanged().AddSP( this, &SOdysseyAnimationCellImageVector::CreateCheckerboardTexture );

    CreateCheckerboardTexture();

    //---

    ChildSlot
    .VAlign(VAlign_Center)
    .HAlign(HAlign_Left)
    [
        SNew(SHorizontalBox)
        .Clipping(EWidgetClipping::ClipToBoundsAlways)
        .Visibility(this, &SOdysseyAnimationCellImageVector::GetContentVisibility)
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
SOdysseyAnimationCellImageVector::RefreshCellRenderTarget()
{
    FTextureRenderTargetResource* RTResource = RenderTarget->GameThread_GetRenderTargetResource();
    ThumbnailTools::RenderThumbnail( mCell, mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE ), THUMBNAIL_SIZE, ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, RTResource );
}

void
SOdysseyAnimationCellImageVector::CreateCheckerboardTexture()
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
SOdysseyAnimationCellImageVector::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    if (!mCell)
        return;

    if (iEvent.IsInteractive())
        return;

    TArray<FGuid> composition = mCell->GetRenderingComposition(EOdysseyRenderingType::Render, 0);
    if (composition.Contains(iEvent.GetId()))
        RefreshCellRenderTarget();
}

EVisibility
SOdysseyAnimationCellImageVector::GetContentVisibility() const
{
    return mShowContent.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}
