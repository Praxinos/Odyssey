// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTextureImport.h"

#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "ImageUtils.h"
#include "OdysseyPixelFormat.h"
#include "ULISLoaderModule.h"
#include "OdysseyRasterBlockMutator.h"
#include "Factories/TextureFactory.h"

#define LOCTEXT_NAMESPACE "FOdysseyPainterEditorTextureImport"

//---

FOdysseyPainterEditorTextureImport::FOdysseyPainterEditorTextureImport()
{
}

TArray<UOdysseyTextureLayerImageRaster*>
FOdysseyPainterEditorTextureImport::ImportTextures(UTexture2D* DestinationTexture, TArray<UTexture2D*> InputTextures, UOdysseyTextureLayer* ParentLayer, int IndexInParent)
{
    if ( InputTextures.Num() <= 0 || !DestinationTexture || (ParentLayer && ParentLayer->GetTexture() != DestinationTexture))
        return {};

    UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(DestinationTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    if (!textureUserData)
        return {};

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureUserData->GetLayerStack());
    if ( !layerStack )
        return {};

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures"));
    layerStack->Modify();


    FScopedSlowTask progressBar(InputTextures.Num(), LOCTEXT("texture-editor.import-texture-dialog.progress-bar.title", "Importing Textures"));
    progressBar.MakeDialog();

    //---

    TArray<UOdysseyLayer*> layers = layerStack->AddLayers(UOdysseyTextureLayerImageRaster::StaticClass(), ParentLayer, IndexInParent, InputTextures.Num());
    TArray<UOdysseyTextureLayerImageRaster*> rasterLayers;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
    renderTarget->bForceLinearGamma = false;
    FIntRect dstRect = layerStack->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(dstRect.Width(), dstRect.Height());


    for( int i = 0; i < layers.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layers[i]);
        UTexture2D* texture = InputTextures[i];
        texture->BlockOnAnyAsyncBuild();
        texture->SetForceMipLevelsToBeResident(1.0f); //ensure Mip 0 is usable
        texture->WaitForStreaming();

        FIntRect srcRect(0, 0, texture->GetSurfaceWidth(), texture->GetSurfaceHeight());

        const ERHIFeatureLevel::Type featureLevel = GMaxRHIFeatureLevel;

        ENQUEUE_RENDER_COMMAND(ImportTextureSequence)(
            [renderTarget, texture, dstRect, srcRect, featureLevel](FRHICommandListImmediate& RHICmdList)
            {
                FRDGBuilder graphBuilder(RHICmdList);

                FRDGTextureRef renderTargetTexture = renderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );
                FRDGTextureRef externalTexture = graphBuilder.RegisterExternalTexture(CreateRenderTarget(texture->GetResource()->TextureRHI, TEXT("UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportTextureSequence::texture")));

                AddClearRenderTargetPass(graphBuilder, renderTargetTexture, FLinearColor::Transparent, dstRect);

                const FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);

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

        rasterLayers.Add(layer);

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, rasterBlock->GetWidth(), rasterBlock->GetHeight());
        rasterBlockMutator.Copy(block, { invalidRect });
        rasterBlockMutator.Commit();
    }

    //---

    return rasterLayers;
}

TArray<UOdysseyTextureLayerImageRaster*>
FOdysseyPainterEditorTextureImport::ImportImages(UTexture2D* DestinationTexture, TArray<FString> Paths, UOdysseyTextureLayer* ParentLayer, int IndexInParent)
{
    if ( Paths.Num() <= 0 || !DestinationTexture || (ParentLayer && ParentLayer->GetTexture() != DestinationTexture))
        return {};

    FScopedTransaction ScopedTransaction(LOCTEXT("texture-editor.transaction.import-images", "Import Images"));

    FScopedSlowTask progressBar(Paths.Num(), LOCTEXT("texture-editor.import-images.progress-bar.title", "Importing Images"));
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

    TArray<UTexture2D*> textures;

    for (int i = 0; i < importedTextures.Num(); i++)
    {
        textures.Add(importedTextures[i].Get());
    }

    return ImportTextures(DestinationTexture, textures, ParentLayer, IndexInParent);
}

#undef LOCTEXT_NAMESPACE
