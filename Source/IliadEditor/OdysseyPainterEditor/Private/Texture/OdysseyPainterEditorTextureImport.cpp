// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorTextureImport.h"

#include "Misc/ScopedSlowTask.h"
#include "ScopedTransaction.h"

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


TArray<UOdysseyTextureLayerImageRaster*>
FOdysseyPainterEditorTextureImport::ImportTextures(UTexture2D* DestinationTexture, const FOdysseyImportTexturesParameters& iImportData)
{
    if ( iImportData.GetSourceTextures().Num() <= 0 || !DestinationTexture )
        return {};

    UOdysseyTextureLayerStackUserData* textureUserData = Cast<UOdysseyTextureLayerStackUserData>(DestinationTexture->GetAssetUserDataOfClass(UOdysseyTextureLayerStackUserData::StaticClass()));
    if (!textureUserData)
        return {};

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureUserData->GetLayerStack());
    if ( !layerStack )
        return {};

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures"));
    layerStack->Modify();


    FScopedSlowTask progressBar(iImportData.GetSourceTextures().Num(), LOCTEXT("texture-editor.import-texture-dialog.progress-bar.title", "Importing Textures"));
    progressBar.MakeDialog();

    //---

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    UOdysseyLayer* parentLayer = nullptr;
    int indexInParent = 0;
    if (currentLayer)
    {
        parentLayer = currentLayer->GetParent();
        indexInParent = currentLayer->GetIndexInParent();
    }

    TArray<UOdysseyLayer*> layers = layerStack->AddLayers(UOdysseyTextureLayerImageRaster::StaticClass(), parentLayer, indexInParent, iImportData.GetSourceTextures().Num());
    TArray<UOdysseyTextureLayerImageRaster*> rasterLayers;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(iImportData.CreateRT());
    for( int i = 0; i < layers.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layers[i]);
        layer->SetLayerName(FText::FromString(iImportData.GetTextureName(i)));
        iImportData.Render(renderTarget.Get(), i);

        //PATCH: Needed to avoid double sRGB application in GetRenderTargetImage()
        if (DestinationTexture->SRGB)
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
            renderTarget->bForceLinearGamma = false;
        }
        //PATCH: End

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
            continue;

        //PATCH: Needed to avoid double sRGB application in GetRenderTargetImage()
        if (DestinationTexture->SRGB)
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8;
            renderTarget->bForceLinearGamma = true;
        }
        //PATCH: End

        ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
        TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( OutImage.SizeX, OutImage.SizeY, format ));
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

#undef LOCTEXT_NAMESPACE
