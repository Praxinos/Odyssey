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

    TArray<UOdysseyLayer*> layers = layerStack->AddLayers(UOdysseyTextureLayerImageRaster::StaticClass(), nullptr, INDEX_NONE, iImportData.GetSourceTextures().Num());
    TArray<UOdysseyTextureLayerImageRaster*> rasterLayers;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(iImportData.CreateRT());
    for( int i = 0; i < layers.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layers[i]);
        iImportData.Render(renderTarget.Get(), i);

        FImage OutImage;
        if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
            continue;

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
