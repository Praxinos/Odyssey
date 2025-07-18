// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageRasterShortcuts.h"

#include "Widgets/Animation/Timeline/Cells/CellImageStagger/SOdysseyAnimationCellImageStagger.h"
#include "Widgets/Animation/Timeline/Cells/CellImageRaster/SOdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyAnimation.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "Shortcuts/AnimationTimeline/OdysseyAnimationTimelineCellImageStaggerShortcuts.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyLayerCellSelection.h"
#include "ScopedTransaction.h"
#include "RenderGraphBuilder.h"
#include "ScreenPass.h"
#include "OdysseyBlendShader.h"
#include "OdysseyPixelFormat.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "ImageUtils.h"
#include "Engine/TextureRenderTarget2D.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

FOdysseyAnimationTimelineCellImageRasterShortcuts::FOdysseyAnimationTimelineCellImageRasterShortcuts(const TAttribute<UOdysseyAnimation*>& iAnimation)
    : mAnimation(iAnimation)
{
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().CrossFade,
        FExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade),
        FCanExecuteAction::CreateRaw(this, &FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade)
    );
}

void
FOdysseyAnimationTimelineCellImageRasterShortcuts::Action_CrossFade()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return;

    UOdysseyLayerStack* layerStack = animation->GetLayerStack();
    if (!layerStack)
        return;

    UOdysseyLayer* layer = layerStack->GetCurrentLayer();
    if (!layer)
        return;

    if (!layer->IsEditable())
        return;

    TArray<UOdysseyLayerCell*> selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->GetExposure() > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return;

    /* ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    } */

#if WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("cell-image-raster.transaction.cross-fade", "Cross Fade"));
#endif

    FScopedSlowTask progressBar(2, LOCTEXT("cell-image-raster.cross-fade.progress-bar.title", "Applying Cross Fade"));
    progressBar.MakeDialog();
    progressBar.EnterProgressFrame();

    //Convert Selected Stagger Cells to ImageRaster Cells
    FOdysseyAnimationTimelineCellImageStaggerShortcuts staggerShortcuts(animation);
    staggerShortcuts.Action_ConvertToReferenceCells();

    progressBar.EnterProgressFrame();

    //Get the selected cells again to ensure having the converted cells
    selectedCells = layerStack->GetCellSelection()->GetSelectedCells();
    TArray<UOdysseyLayerCell*> cellsToSelect = selectedCells;

    //Cross Fade all selected cells
    //::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

    FIntRect rect = layerStack->GetDefaultRenderRect();
    TStrongObjectPtr<UTextureRenderTarget2D> startRenderTarget(NewObject<UTextureRenderTarget2D>());
    TStrongObjectPtr<UTextureRenderTarget2D> endRenderTarget(NewObject<UTextureRenderTarget2D>());
    TStrongObjectPtr<UTextureRenderTarget2D> destinationRenderTarget(NewObject<UTextureRenderTarget2D>());
    startRenderTarget->InitAutoFormat(rect.Width(), rect.Height());
    endRenderTarget->InitAutoFormat(rect.Width(), rect.Height());
    destinationRenderTarget->InitAutoFormat(rect.Width(), rect.Height());

    FScopedSlowTask selectedCellsProgress(selectedCells.Num());
    for (UOdysseyLayerCell* selectedCell : selectedCells)
    {
        selectedCellsProgress.EnterProgressFrame();
        selectedCell->Render_GameThread(
            startRenderTarget.Get(),
            FFrameNumber(0),
            EOdysseyRenderingType::Render
        );

        int cellIndex = selectedCell->GetIndexInLayer();
        int nextCellIndex = cellIndex + 1;
        int crossFadelength = selectedCell->GetExposure();

        //Reduce the original cell to 1 frame Exposure
        selectedCell->SetExposure(1);

        if (nextCellIndex < layer->GetCells().Num() || !layer->GetCells()[nextCellIndex])
        {
            UOdysseyLayerCell* nextCell = layer->GetCells()[nextCellIndex];

            nextCell->Render_GameThread(
                endRenderTarget.Get(),
                FFrameNumber(0),
                EOdysseyRenderingType::Render
            );

            TArray<UOdysseyLayerCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->GetIndexInLayer() + 1, crossFadelength - 1);

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[j - 1]);

                const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

                ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
                    [startRenderTarget, endRenderTarget, destinationRenderTarget, featureLevel, rect, crossFadelength, j](FRHICommandListImmediate& RHICmdList)
                    {
                        FRDGBuilder graphBuilder(RHICmdList);

                        FRDGTextureRef startTexture = startRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                        FRDGTextureRef endTexture = endRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                        FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                        AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, rect);

                        FOdysseyBlendShader::BlendRect(
                            graphBuilder,
                            featureLevel,
                            destinationTexture,
                            startTexture,
                            destinationTexture,
                            rect,
                            rect,
                            FMatrix::Identity,
                            EOdysseyBlendingMode::kAdd,
                            EOdysseyAlphaMode::kAdd,
                            1.f - (float(j) / float(crossFadelength)),
                            EOdysseyAntiAliasing::NearestNeighbor
                        );

                        FOdysseyBlendShader::BlendRect(
                            graphBuilder,
                            featureLevel,
                            destinationTexture,
                            endTexture,
                            destinationTexture,
                            rect,
                            rect,
                            FMatrix::Identity,
                            EOdysseyBlendingMode::kAdd,
                            EOdysseyAlphaMode::kAdd,
                            float(j) / float(crossFadelength),
                            EOdysseyAntiAliasing::NearestNeighbor
                        );

                        graphBuilder.Execute();
                    }
                );

                FImage OutImage;
                if (!FImageUtils::GetRenderTargetImage(destinationRenderTarget.Get(), OutImage))
                    return;

                ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);

                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
                TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
                CopyImageToBlock(OutImage, block.Get());

                FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
                mutator.Copy(block, {::ULISUtils::ToULISRectI(rect)});
                mutator.Commit();
            }

            cellsToSelect.Append(rasterCells);
        }
        else
        {
            TArray<UOdysseyLayerCell*> rasterCells = layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), selectedCell->GetIndexInLayer() + 1, crossFadelength - 1);

            FScopedSlowTask framesProgress(crossFadelength);
            for (int j = 1; j < crossFadelength; j++)
            {
                framesProgress.EnterProgressFrame();

                UOdysseyAnimationCellImageRaster* rasterCell = Cast<UOdysseyAnimationCellImageRaster>(rasterCells[j - 1]);

                const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

                ENQUEUE_RENDER_COMMAND(IOdysseyTextureRenderingAbility_RenderRectAtRect)(
                    [startRenderTarget, endRenderTarget, destinationRenderTarget, featureLevel, rect, crossFadelength, j](FRHICommandListImmediate& RHICmdList)
                    {
                        FRDGBuilder graphBuilder(RHICmdList);

                        FRDGTextureRef startTexture = startRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                        FRDGTextureRef destinationTexture = destinationRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

                        AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent, rect);

                        FOdysseyBlendShader::BlendRect(
                            graphBuilder,
                            featureLevel,
                            destinationTexture,
                            startTexture,
                            destinationTexture,
                            rect,
                            rect,
                            FMatrix::Identity,
                            EOdysseyBlendingMode::kAdd,
                            EOdysseyAlphaMode::kAdd,
                            1.f - (float(j) / float(crossFadelength)),
                            EOdysseyAntiAliasing::NearestNeighbor
                        );

                        graphBuilder.Execute();
                    }
                );

                FImage OutImage;
                if (!FImageUtils::GetRenderTargetImage(destinationRenderTarget.Get(), OutImage))
                    return;

                ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);

                ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
                TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
                CopyImageToBlock(OutImage, block.Get());

                FOdysseyRasterBlockMutator mutator(rasterCell->GetRasterBlock(), false);
                mutator.Copy(block, {::ULISUtils::ToULISRectI(rect)});
                mutator.Commit();
            }
            cellsToSelect.Append(rasterCells);
        }
    }
    layerStack->GetCellSelection()->SetSelectedCells(cellsToSelect);
}

bool
FOdysseyAnimationTimelineCellImageRasterShortcuts::CanAction_CrossFade()
{
    UOdysseyAnimation* animation = mAnimation.Get();
    if (!animation)
        return false;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(animation->GetLayerStack());
    if (!layerStack)
        return false;

    UOdysseyAnimationLayer* layer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
    if (!layer || !layer->IsA(UOdysseyAnimationLayerImageRaster::StaticClass()))
        return false;

    if (!layer->IsEditable())
        return false;

    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->GetCurrentLayer());

    const TArray<UOdysseyLayerCell*> selectedCells = layerImageRaster->GetLayerStack()->GetCellSelection()->GetSelectedCells();
    if (selectedCells.IsEmpty())
        return false;

    bool hasCellsToCrossFade = selectedCells.ContainsByPredicate(
        [](UOdysseyLayerCell* iCell)
        {
            return iCell->GetExposure() > 1;
        }
    );
    if (!hasCellsToCrossFade)
        return false;

    return true;
}


#undef LOCTEXT_NAMESPACE
