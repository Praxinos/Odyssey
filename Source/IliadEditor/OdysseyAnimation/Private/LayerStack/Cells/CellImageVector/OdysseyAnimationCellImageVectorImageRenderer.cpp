// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVectorImageRenderer.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorBlock.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimation.h"
#include "OdysseyLayerStack.h"

#include "ULISUtils.h"
#include "ULISEventBuilder.h"

static FCriticalSection mEngineMutex;

FOdysseyAnimationCellImageVectorImageRenderer::FOdysseyAnimationCellImageVectorImageRenderer(const UOdysseyAnimationCellImageVector* iCell, int iFrame, EOdysseyRenderingType iRenderType, const TArray<FIntRect>& iDefaultRects, FImageRendererFilter iFilter)
    : IOdysseyImageRenderer(iRenderType, iDefaultRects)
    , mCell(iCell)
    , mBlock(nullptr)
    , mHUDBlock(nullptr)
    , mDrawingFlags(0)
    , mRenderHUD(false)
{
    /* TEMPORARY FOR DISPLAYING TOOLS HUD */
    UOdysseyAnimationLayerImageVector* layer = Cast<UOdysseyAnimationLayerImageVector>(mCell->GetLayer());
    if (layer)
    {
        UOdysseyAnimation* animation = layer->GetAnimation();
        if (animation)
        {
            UOdysseyAnimationCell* cell = layer->GetCellAtFrame(animation->CurrentFrame);
            if (cell)
            {
                UOdysseyLayerStack* layerStack = layer->GetLayerStack();
                int frame = animation->CurrentFrame - cell->GetFrameRange().GetLowerBoundValue();
                mRenderHUD = ( cell == mCell )
                        && ( frame == iFrame )
                        && ( layerStack->CurrentLayer.Get() == layer )
                        && ( GetRenderType() == EOdysseyRenderingType::Editor );
            }
        }

        // this is per-layer
        mDrawingFlags  = layer->IsColored   ? mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_IGNORECOLOR)
                                            : mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_IGNORECOLOR);
        // TEMP: this should be global, stored in PainterEditor. Hence this should be
        // changed when PainterEditor will be available and we can retrieve the shared flags.
        // Update: commented-out for now
        mDrawingFlags |= layer->IsWireframe ? mDrawingFlags | ( FOdysseyVectorEngine::DRAWING_WIREFRAME)
                                            : mDrawingFlags & (~FOdysseyVectorEngine::DRAWING_WIREFRAME);

        FVector2D outOfPegsPan = mCell->OutOfPegs.Pan;
        float outOfPegsRotation = mCell->OutOfPegs.Rotation;
        float outOfPegsZoom = mCell->OutOfPegs.Zoom;

        mOutOfPegsTransform = ::ULIS::FMat3F::MakeTranslationMatrix(animation->GetWidth() / 2.f, animation->GetHeight() / 2.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix(outOfPegsPan.X, outOfPegsPan.Y)
            * ::ULIS::FMat3F::MakeRotationMatrix(FMath::DegreesToRadians(outOfPegsRotation))
            * ::ULIS::FMat3F::MakeScaleMatrix(outOfPegsZoom / 100.f, outOfPegsZoom / 100.f)
            * ::ULIS::FMat3F::MakeTranslationMatrix( animation->GetWidth() / -2.f, animation->GetHeight() / -2.f);
    }
}

void
FOdysseyAnimationCellImageVectorImageRenderer::Init()
{
    TSharedPtr<FOdysseyVectorBlock> vectorBlock = mCell->GetVectorBlock();

    if (vectorBlock)
    {
        mBlock = vectorBlock->GetBlock( mDrawingFlags ); //Store block before rendering to avoid looking twice for the block in cache
        /* TEMPORARY FOR DISPLAYING TOOLS HUD */
        if (mRenderHUD)
            mHUDBlock = vectorBlock->GetHUDBlock();
    }

    {
        vectorBlock->Render( mDrawingFlags );
    }
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Blend(const FOdysseyImageRendererBlendParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    FOdysseyImageRendererBlendParams params(iParams);
    if (GetRenderType() == EOdysseyRenderingType::RenderOutOfPegs)
        params.mTransform = mOutOfPegsTransform;

    TArray<::ULIS::FEvent> events = ConvertAndBlend(mBlock, ::ULIS::FVec2I(0), params, iWaitList);

    if (!mHUDBlock)
        return events;

    FOdysseyImageRendererBlendParams hudParams = iParams;
    hudParams.mBlendMode = ::ULIS::Blend_Normal;
    hudParams.mOpacity = 1.f;

    return ConvertAndBlend(mHUDBlock, ::ULIS::FVec2I(0), hudParams, events);
}

TArray<::ULIS::FEvent>
FOdysseyAnimationCellImageVectorImageRenderer::Copy(const FOdysseyImageRendererCopyParams& iParams, const TArray<::ULIS::FEvent>& iWaitList)
{
    if (!mBlock)
        return iWaitList;

    FOdysseyImageRendererCopyParams params(iParams);
    if (GetRenderType() == EOdysseyRenderingType::RenderOutOfPegs)
        params.mTransform = mOutOfPegsTransform;

    return ConvertAndCopy(mBlock, ::ULIS::FVec2I(0), params, iWaitList);
}

void
FOdysseyAnimationCellImageVectorImageRenderer::Lock()
{
    mCell->GetImageRenderingMutex()->Lock();
}

void
FOdysseyAnimationCellImageVectorImageRenderer::Unlock()
{
    mCell->GetImageRenderingMutex()->Unlock();
}

bool
FOdysseyAnimationCellImageVectorImageRenderer::IsGameThreadOnly()
{
    return mCell->IsImageRenderingGameThreadOnly();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationCellImageVectorImageRenderer::AddReferencedObjects(FReferenceCollector& Collector)
{
    Collector.AddReferencedObject(mCell);
}

FString
FOdysseyAnimationCellImageVectorImageRenderer::GetReferencerName() const
{
    return "FOdysseyAnimationCellImageVectorImageRenderer";
}
