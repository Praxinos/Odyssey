// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorAnimationImport.h"

#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ImageUtils.h"
#include "ScreenPass.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "SOdysseyAnimationLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "SOdysseyAnimationExportImageSequenceDialog.h"
#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "ScopedTransaction.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"
#include "Factories/TextureFactory.h"
#include "Factories/Factory.h"

#define LOCTEXT_NAMESPACE "FOdysseyPainterEditorAnimationImport"

namespace
{
//PATCH: Needed to avoid double sRGB application in GetRenderTargetImage()
// The render target is shared between imports (FOdysseyImportTexturesSharedRT),
// so its original settings must always be restored, including on early returns
struct FScopedSRGBReadbackPatch
{
    FScopedSRGBReadbackPatch( UTextureRenderTarget2D* iRenderTarget, bool iEnabled )
        : mRenderTarget( iEnabled ? iRenderTarget : nullptr )
    {
        if( !mRenderTarget )
            return;

        mPreviousFormat = mRenderTarget->RenderTargetFormat;
        mPreviousForceLinearGamma = mRenderTarget->bForceLinearGamma;
        mRenderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
        mRenderTarget->bForceLinearGamma = false;
    }

    ~FScopedSRGBReadbackPatch()
    {
        if( !mRenderTarget )
            return;

        mRenderTarget->RenderTargetFormat = mPreviousFormat;
        mRenderTarget->bForceLinearGamma = mPreviousForceLinearGamma;
    }

    UTextureRenderTarget2D* mRenderTarget;
    TEnumAsByte<ETextureRenderTargetFormat> mPreviousFormat = RTF_RGBA8;
    uint8 mPreviousForceLinearGamma = true;
};
//PATCH: End
}

FOdysseyPainterEditorAnimationImportResult
FOdysseyPainterEditorAnimationImport::ImportTextureSequence(UOdysseyAnimation* Animation, const FOdysseyImportTexturesParameters& iImportData)
{
    FOdysseyPainterEditorAnimationImportResult result;

    if ( iImportData.GetSourceTextures().Num() <= 0 || !Animation )
        return result;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(Animation->GetLayerStack());
    if ( !layerStack )
        return result;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    UOdysseyLayer* parentLayer = nullptr;
    int indexInParent = 0;
    if (currentLayer)
    {
        parentLayer = currentLayer->GetParent();
        indexInParent = currentLayer->GetIndexInParent();
    }

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parentLayer, indexInParent);
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    FScopedSlowTask progressBar(iImportData.GetSourceTextures().Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    result = FOdysseyPainterEditorAnimationLayerImport::ImportTextureSequence(layerImageRaster, iImportData, 0);

    result.mImportedLayers.Add(layerImageRaster);

    return result;
}

FOdysseyPainterEditorAnimationImportResult
FOdysseyPainterEditorAnimationLayerImport::ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, const FOdysseyImportTexturesParameters& iImportData, int iCellIndex)
{
    FOdysseyPainterEditorAnimationImportResult result;

    if ( iImportData.GetSourceTextures().Num() <= 0 || !Layer)
        return result;

    UOdysseyAnimation* animation = Layer->GetAnimation();
    if( !animation )
        return result;

    if (iCellIndex != INDEX_NONE)
        iCellIndex = FMath::Clamp(iCellIndex, 0, Layer->GetCells().Num());

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));

    FScopedSlowTask progressBar(iImportData.GetSourceTextures().Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    TArray<UOdysseyLayerCell*> cells = Layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), iCellIndex, iImportData.GetSourceTextures().Num());
    TArray<UOdysseyAnimationCellImageRaster*> rasterCells;

    UTextureRenderTarget2D* renderTarget = FOdysseyImportTexturesSharedRT::Get(iImportData);
    const bool patchSRGB = animation->GetFormat() == EOdysseyAnimationFormat::BGRA8;
    for( int i = 0; i < cells.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        iImportData.Render(renderTarget, i);

        FImage OutImage;
        {
            FScopedSRGBReadbackPatch srgbPatch(renderTarget, patchSRGB);
            if (!FImageUtils::GetRenderTargetImage(renderTarget, OutImage))
                continue;
        }

        ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( renderTarget->GetSurfaceWidth(), renderTarget->GetSurfaceHeight(), format ));
        CopyImageToBlock(OutImage, block.Get());

        result.mImportedCells.Add(cell);

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
        rasterBlockMutator.Copy(block, { invalidRect });
        rasterBlockMutator.Commit();
    }

    return result;
}

FOdysseyPainterEditorAnimationImportResult
FOdysseyPainterEditorAnimationCellImport::ImportTextureSequence( UOdysseyAnimationCellImageRaster* iCell, const FOdysseyImportTexturesParameters& iImportData)
{
    FOdysseyPainterEditorAnimationImportResult result;

    if ( iImportData.GetSourceTextures().Num() <= 0 || !iCell )
        return result;

    UOdysseyAnimation* animation = iCell->GetAnimation();
    if( !animation )
        return result;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));

    UTextureRenderTarget2D* renderTarget = FOdysseyImportTexturesSharedRT::Get(iImportData);

    iImportData.Render(renderTarget, 0); // For cell, always get only the first texture

    FImage OutImage;
    {
        FScopedSRGBReadbackPatch srgbPatch(renderTarget, animation->GetFormat() == EOdysseyAnimationFormat::BGRA8);
        if (!FImageUtils::GetRenderTargetImage(renderTarget, OutImage))
            return result;
    }

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( renderTarget->GetSurfaceWidth(), renderTarget->GetSurfaceHeight(), format ));
    CopyImageToBlock(OutImage, block.Get());

    UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>( iCell );

    result.mImportedCells.Add(cell);

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
    FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
    ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
    rasterBlockMutator.Copy( block, { invalidRect } );
    rasterBlockMutator.Commit();

    return result;
}

#undef LOCTEXT_NAMESPACE
