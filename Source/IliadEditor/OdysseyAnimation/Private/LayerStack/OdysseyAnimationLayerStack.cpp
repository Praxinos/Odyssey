// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerRoot/OdysseyAnimationLayerRoot.h"
#include "LayerStack/Layers/LayerFolder/OdysseyAnimationLayerFolder.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "OdysseyRectUtils.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Cells/OdysseyAnimationCellSelection.h"

//===============================================

UOdysseyAnimationLayerStack::UOdysseyAnimationLayerStack()
    : mCellSelection(MakeShared<FOdysseyAnimationCellSelection>(this))
{
    CompatibleLayers.Add(UOdysseyAnimationLayerFolder::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageRaster::StaticClass());
    CompatibleLayers.Add(UOdysseyAnimationLayerImageVector::StaticClass());

    LayerRootClass = UOdysseyAnimationLayerRoot::StaticClass();
}

void
UOdysseyAnimationLayerStack::PostInitProperties()
{
    Super::PostInitProperties();

    if (HasAnyFlags(RF_ClassDefaultObject))
        return;

    FOdysseyRenderingAbility::OnRenderingChangedDelegate().AddUObject(this, &UOdysseyAnimationLayerStack::OnRenderingChanged);
}

int
UOdysseyAnimationLayerStack::GetWidth() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetWidth();

    return animation->GetWidth();
}

int
UOdysseyAnimationLayerStack::GetHeight() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetHeight();

    return animation->GetHeight();
}

::ULIS::eFormat
UOdysseyAnimationLayerStack::GetFormat() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return Super::GetFormat();

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    return format;
}

UOdysseyAnimation*
UOdysseyAnimationLayerStack::GetAnimation() const
{
    UObject* outer = GetOuter();
    while(outer)
    {
        if (outer->GetClass() == UOdysseyAnimation::StaticClass())
            return Cast<UOdysseyAnimation>(outer);

        outer = outer->GetOuter();
    }

    return nullptr;
}

FInt32Range
UOdysseyAnimationLayerStack::GetFrameRange() const
{
    return Cast<UOdysseyAnimationLayerRoot>(LayerRoot)->GetFrameRange();
}

TArray<FIntRect>
UOdysseyAnimationLayerStack::GetRenderingRects() const
{
    UOdysseyAnimation* animation = GetAnimation();
    if (!animation)
        return {};

    return { FIntRect(0, 0, animation->GetWidth(), animation->GetHeight()) };
}

TSharedRef<FOdysseyAnimationCellSelection>
UOdysseyAnimationLayerStack::GetCellSelection() const
{
    return mCellSelection;
}

void
UOdysseyAnimationLayerStack::RenderToTextureFromRects(UTextureRenderTarget2D* iRenderTarget, FFrameNumber iFrame, const TArray<FIntRect>& iRects) const
{
    /*
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationLayerStack::CopyBlocksToTexture);
    if ( iBlocks.IsEmpty() )
        return;

    FTextureCompilingManager::Get().FinishCompilation({ Texture });

    ::ULIS::eFormat format = GetFormat();

    //convert block to BGRA8 if needed*
    if ( format == ::ULIS::Format_BGRA8 )
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationPlayer::CopyBlocks);
        TArray<TSharedPtr<FUpdateTextureRegion2D>> regions; //Keeps region object alive until fence.Wait()
        for ( int i = 0; i < iBlocks.Num(); i++ )
        {
            const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
            const ::ULIS::FRectI& rect = iRects[i];
            regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, block->Rect().w, block->Rect().h));
            Texture->UpdateTextureRegions(0, 1, regions.Last().Get(), block->BytesPerScanLine(), block->BytesPerPixel(), block->Bits());
        }

        FRenderCommandFence fence;
        fence.BeginFence();
        fence.Wait();

        return;
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    TArray<TSharedPtr<::ULIS::FBlock>> convBlocks;
    TArray<TSharedPtr<FUpdateTextureRegion2D>> regions;
    for ( int i = 0; i < iBlocks.Num(); i++ )
    {
        const TSharedPtr<::ULIS::FBlock>& block = iBlocks[i];
        const ::ULIS::FRectI& rect = iRects[i];

        TSharedPtr<::ULIS::FBlock> convBlock = MakeShared<::ULIS::FBlock>(rect.w, rect.h, ::ULIS::Format_BGRA8);
        ctx.ConvertFormat(*block, *convBlock, block->Rect(), ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient);
        convBlocks.Add(convBlock);
        regions.Add(MakeShared<FUpdateTextureRegion2D>(rect.x, rect.y, 0, 0, rect.w, rect.h));
    }

    ctx.Finish();

    for ( int i = 0; i < regions.Num(); i++ )
    {
        Texture->UpdateTextureRegions(
            0,
            1,
            regions[i].Get(),
            convBlocks[i]->BytesPerScanLine(),
            convBlocks[i]->BytesPerPixel(),
            convBlocks[i]->Bits()
        );
    }

    FRenderCommandFence fence;
    fence.BeginFence();
    fence.Wait(); */
}

void
UOdysseyAnimationLayerStack::OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimationLayerStack::OnRenderingChanged);
    if (!iEvent.IsInteractive())
    {
        const FGuid& eventId = iEvent.GetId();
        FInt32Range frameRange = GetFrameRange();
        int startFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetLowerBoundValue() : frameRange.GetLowerBoundValue() + 1;
        int endFrame = frameRange.GetUpperBound().IsInclusive() ? frameRange.GetUpperBoundValue() : frameRange.GetUpperBoundValue() - 1;
        for (int i = startFrame; i <= endFrame; i++)
        {
            TArray<FGuid> composition = GetRenderingComposition(EOdysseyRenderingType::Editor, i);
            if (composition.Contains(eventId))
            {
                MarkPackageDirty();
                break;
            }
        }
    }
}
