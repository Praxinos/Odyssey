// IDDN.FR.001.060015.014.S.X.2019.000.00000
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

FOdysseyPainterEditorAnimationImportResult
FOdysseyPainterEditorAnimationImport::ImportTextureSequence(UOdysseyAnimation* Animation, const FOdysseyImportTexturesParameters& iImportData)
{
    FOdysseyPainterEditorAnimationImportResult result;

    if ( iImportData.GetSourceTextures().Num() <= 0 || !Animation )
        return result;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(Animation->GetLayerStack());
    if ( !layerStack )
        return result;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
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

    UTextureRenderTarget2D* renderTarget = iImportData.CreateRT();
    for( int i = 0; i < cells.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        iImportData.Render(renderTarget, i);

        //PATCH: Needed to avoid double sRGB application in GetRenderTargetImage()
        if (animation->GetFormat() == EOdysseyAnimationFormat::BGRA8)
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
            renderTarget->bForceLinearGamma = false;
        }
        //PATCH: End

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(renderTarget, OutImage))
            continue;

        //PATCH: Needed to avoid double sRGB application in GetRenderTargetImage()
        if (animation->GetFormat() == EOdysseyAnimationFormat::BGRA8)
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8;
            renderTarget->bForceLinearGamma = true;
        }
        //PATCH: End

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

#undef LOCTEXT_NAMESPACE
