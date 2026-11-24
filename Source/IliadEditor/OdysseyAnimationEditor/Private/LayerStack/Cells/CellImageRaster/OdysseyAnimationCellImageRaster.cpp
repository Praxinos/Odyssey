// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyRenderContext.h"

#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "OdysseyAnimation.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "UObject/StrongObjectPtr.h"
#include "OdysseyTiledImage.h"
#include "ULISLoaderModule.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyAnimationCellImageRasterImport.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyRasterBlock.h"
#include "UObject/ObjectSaveContext.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "Animation"

UOdysseyAnimationCellImageRaster::UOdysseyAnimationCellImageRaster() = default;

TArray<FGuid>
UOdysseyAnimationCellImageRaster::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    return { GetRenderingId() };
}

FIntRect
UOdysseyAnimationCellImageRaster::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetAnimation()->GetWidth(), GetAnimation()->GetHeight());
}

bool
UOdysseyAnimationCellImageRaster::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    return GetRasterContent().BuildRenderFunction(oRenderFunction, GetContentTransform(RenderContext));
}

const UOdysseyTiledImage*
UOdysseyAnimationCellImageRaster::GetTiledImage() const
{
    return TiledImage;
}

void
UOdysseyAnimationCellImageRaster::UpdateContent()
{
    FOdysseyRasterContent& Content = GetRasterContent();
    Content.MigrateLegacy(Texture, mLegacyPixels);
    Content.EnsureImage();
}

FOdysseyRasterContent&
UOdysseyAnimationCellImageRaster::GetRasterContent() const
{
    UOdysseyAnimationCellImageRaster* MutableThis = const_cast<UOdysseyAnimationCellImageRaster*>(this);
    if (!mRasterContent)
    {
        // As the former render texture, 8-bit raster blocks of cells hold sRGB encoded colors; the tiles hold them decoded.
        mRasterContent = MakeShared<FOdysseyRasterContent>(*MutableThis, MutableThis->GetRenderingId(), MutableThis->TiledImage,
            [MutableThis]()
            {
                FOdysseyRasterContent::FDescription Description;
                if (MutableThis->GetAnimation())
                {
                    Description.Size = MutableThis->GetDefaultRenderRect().Size();
                    Description.Format = MutableThis->GetRasterFormat();
                    Description.bSRGB = true;
                }
                return Description;
            },
            [MutableThis](const TArray<FIntRect>& Rects)
            {
                MutableThis->RenderingChanged(Rects, false);
            },
            [MutableThis]()
            {
                const UOdysseyAnimationLayerImageRaster* Layer = Cast<UOdysseyAnimationLayerImageRaster>(MutableThis->GetLayer());
                return Layer && Layer->IsAlphaLocked();
            });
    }
    return *mRasterContent;
}

::ULIS::eFormat
UOdysseyAnimationCellImageRaster::GetRasterFormat() const
{
    UOdysseyAnimation* Animation = GetAnimation();
    return Animation && Animation->GetFormat() == EOdysseyAnimationFormat::RGBAF ? ::ULIS::Format_RGBAF : ::ULIS::Format_BGRA8;
}

void
UOdysseyAnimationCellImageRaster::PostLoad()
{
    Super::PostLoad();
    UpdateContent();
}

void
UOdysseyAnimationCellImageRaster::PostDuplicate(EDuplicateMode::Type DuplicateMode)
{
    Super::PostDuplicate(DuplicateMode);
    // The animation of the duplicate may have another format.
    UpdateContent();
}

void
UOdysseyAnimationCellImageRaster::BeginDestroy()
{
    mRasterContent.Reset();
    Super::BeginDestroy();
}

FOdysseyMediaProvider
UOdysseyAnimationCellImageRaster::GetMediaProvider() const
{
    TSharedPtr<FOdysseyMediaRaster> mediaRaster = MakeShared<FOdysseyMediaRaster>(GetRenderingId(), GetRasterContent().AsWeak());
    FOdysseyMediaProvider mediaProvider;
    mediaProvider.Add(mediaRaster);
    return mediaProvider;
}

void
UOdysseyAnimationCellImageRaster::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    if( Ar.IsLoading() && !Ar.IsTransacting() )
    {
        FOdysseyAnimationCellImageRasterImport::Read( this, Ar );
    }
}

void
UOdysseyAnimationCellImageRaster::OldSerialize(FArchive& Ar)
{
    Super::OldSerialize(Ar);

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImageRasterImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            mLegacyPixels = Odyssey::ReadLegacyRasterBlock(Ar, *this);
        }
        // PostLoad is not called after OldSerialize: the cell moves its legacy pixels now.
        UpdateContent();
    }
}

void
UOdysseyAnimationCellImageRaster::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    // Content still held by a legacy render texture must reach the tiled image before saving.
    UpdateContent();
}

void
UOdysseyAnimationCellImageRaster::Clear()
{
    FScopedTransaction ScopedTransaction(LOCTEXT("animation-cell-raster.clear", "Clear"));
    GetRasterContent().Clear();
}

#undef LOCTEXT_NAMESPACE
