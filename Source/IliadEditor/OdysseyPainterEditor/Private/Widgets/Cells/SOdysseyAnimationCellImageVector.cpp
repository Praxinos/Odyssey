// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationCellImageVector.h"

#include "Brushes/SlateImageBrush.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"

#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorSettings.h"

#define THUMBNAIL_SIZE_VECTOR 32

namespace
{
    static TStrongObjectPtr<UTexture2D> sVectorCheckerboardTexture;
    static FColor sVectorCheckerboardColorOne = FColor( EForceInit::ForceInit );
    static FColor sVectorCheckerboardColorTwo = FColor( EForceInit::ForceInit );
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

    RenderTarget->InitAutoFormat( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_VECTOR ), THUMBNAIL_SIZE_VECTOR );
    //RenderTarget->ResizeTarget( mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_VECTOR ), THUMBNAIL_SIZE_VECTOR );
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
    ThumbnailTools::RenderThumbnail( mCell, mCell->GetAnimation()->GetWidthFromHeightKeepingRatio( THUMBNAIL_SIZE_VECTOR ), THUMBNAIL_SIZE_VECTOR, ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, RTResource );
}

void
SOdysseyAnimationCellImageVector::CreateCheckerboardTexture()
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    if( !sVectorCheckerboardTexture
        || sVectorCheckerboardColorOne != settings.GetCheckerColorOne()
        || sVectorCheckerboardColorTwo != settings.GetCheckerColorTwo()
        )
    {
        sVectorCheckerboardTexture = TStrongObjectPtr<UTexture2D>( FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), 16 ) );
        sVectorCheckerboardColorOne = settings.GetCheckerColorOne();
        sVectorCheckerboardColorTwo = settings.GetCheckerColorTwo();
    }

    //---

    if( !mCheckerboardBrush )
        mCheckerboardBrush = new FSlateImageBrush( sVectorCheckerboardTexture.Get(), FVector2D( sVectorCheckerboardTexture->GetSurfaceWidth(), sVectorCheckerboardTexture->GetSurfaceHeight() ), FSlateColor( FLinearColor::White ), ESlateBrushTileType::Both );

    mCheckerboardBrush->SetResourceObject( sVectorCheckerboardTexture.Get() );
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
