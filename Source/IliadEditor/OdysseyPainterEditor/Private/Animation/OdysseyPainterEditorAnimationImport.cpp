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
#include "Widgets/Animation/Timeline/SOdysseyAnimationLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Widgets/Animation/SOdysseyAnimationExportImageSequenceDialog.h"
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

//---

FOdysseyPainterEditorAnimationImport::FOdysseyPainterEditorAnimationImport()
{
}

UOdysseyAnimationLayerImageRaster*
FOdysseyPainterEditorAnimationImport::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    if ( Textures.Num() <= 0 || !Animation || (ParentLayer && ParentLayer->GetAnimation() != Animation))
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(Animation->GetLayerStack());
    if ( !layerStack )
        return nullptr;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), ParentLayer, IndexInParent);
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);

    FScopedSlowTask progressBar(Textures.Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    FOdysseyPainterEditorAnimationLayerImport import_layer;
    import_layer.ImportTextureSequence(layerImageRaster, Textures, 0);

    return layerImageRaster;
}

UOdysseyAnimationLayerImageRaster*
FOdysseyPainterEditorAnimationImport::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    if ( Paths.Num() <= 0 || !Animation || (ParentLayer && ParentLayer->GetAnimation() != Animation))
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(Animation->GetLayerStack());
    if ( !layerStack )
        return nullptr;

    FScopedTransaction ScopedTransaction(LOCTEXT("animation-editor.transaction.import-image-sequence", "Import Image Sequence"));
    UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), ParentLayer, IndexInParent));

    FOdysseyPainterEditorAnimationLayerImport import_layer;
    import_layer.ImportImageSequence(layer, Paths, 0);

    return layer;
}

//---

FOdysseyPainterEditorAnimationLayerImport::FOdysseyPainterEditorAnimationLayerImport()
{
}

TArray<UOdysseyAnimationCellImageRaster*>
FOdysseyPainterEditorAnimationLayerImport::ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, int iCellIndex)
{
    if ( Textures.Num() <= 0 || !Layer)
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();
    if (iCellIndex != INDEX_NONE)
        iCellIndex = FMath::Clamp(iCellIndex, 0, Layer->GetCells().Num());

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));

    FScopedSlowTask progressBar(Textures.Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    TArray<UOdysseyLayerCell*> cells = Layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), iCellIndex, Textures.Num());
    TArray<UOdysseyAnimationCellImageRaster*> rasterCells;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
    renderTarget->bForceLinearGamma = false;
    FIntRect dstRect = Layer->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(dstRect.Width(), dstRect.Height());

    for( int i = 0; i < cells.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        UTexture2D* texture = Textures[i];
        texture->BlockOnAnyAsyncBuild();
        FIntRect srcRect(0, 0, texture->GetSurfaceWidth(), texture->GetSurfaceHeight());

        const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

        ENQUEUE_RENDER_COMMAND(ImportTextureSequence)(
            [renderTarget, texture, dstRect, srcRect, featureLevel](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef renderTargetTexture = renderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                FRDGTextureRef externalTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(texture->GetResource()->TextureRHI, TEXT("UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportTextureSequence::texture")));

                AddClearRenderTargetPass(graphBuilder, renderTargetTexture, FLinearColor::Transparent, dstRect);
                AddDrawTexturePass(
                    graphBuilder,
                    FScreenPassViewInfo(),
                    externalTexture,
                    renderTargetTexture,
                    srcRect.Min,
                    srcRect.Size(),
                    dstRect.Min,
                    srcRect.Size()
                );

                //execution du graph
                graphBuilder.Execute();
            }
        );

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
            continue;

        ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( dstRect.Width(), dstRect.Height(), format ));
        CopyImageToBlock(OutImage, block.Get());

        rasterCells.Add(cell);

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
        rasterBlockMutator.Copy(block, { invalidRect });
        rasterBlockMutator.Commit();
    }

    return rasterCells;
}

TArray<UOdysseyAnimationCellImageRaster*>
FOdysseyPainterEditorAnimationLayerImport::ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex)
{
    if ( Paths.Num() <= 0 || !Layer)
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();
    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    if (iCellIndex != INDEX_NONE)
        iCellIndex = FMath::Clamp(iCellIndex, 0, Layer->GetCells().Num());

    FScopedSlowTask progressBar(Paths.Num(), LOCTEXT("animation-editor.import-image-sequence.progress-bar.title", "Importing Image Sequence"));
    progressBar.MakeDialog();

    TStrongObjectPtr<UTextureFactory> TextureFactory(NewObject<UTextureFactory>());
    TArray<TStrongObjectPtr<UTexture2D>> importedTextures;
    importedTextures.Reserve(Paths.Num());
    for (const FString& filename : Paths)
    {
        progressBar.EnterProgressFrame();

        UObject* importedObject = UFactory::StaticImportObject(UTexture2D::StaticClass(), GetTransientPackage(), NAME_None, EObjectFlags::RF_NoFlags, *filename, nullptr, TextureFactory.Get());
        UTexture2D* importedTexture = Cast<UTexture2D>(importedObject);
        if (!importedTexture)
            continue;

        importedTextures.Emplace(importedTexture);
    }

    #if WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("animation-editor.transaction.import-image-sequence", "Import Image Sequence"));
    #endif

    TArray<UTexture2D*> textures;

    for (int i = 0; i < importedTextures.Num(); i++)
    {
        textures.Add(importedTextures[i].Get());
    }

    FOdysseyPainterEditorAnimationLayerImport import_layer;
    return import_layer.ImportTextureSequence(Layer, textures, 0);
}

#undef LOCTEXT_NAMESPACE
