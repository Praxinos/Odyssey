// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorAnimationFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "ScopedTransaction.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"
#include "OdysseyExportImage.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorAnimationFunctionLibrary"

UOdysseyAnimationLayerImageRaster*
UOdysseyPainterEditorAnimationFunctionLibrary::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
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

    UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportTextureSequence(layerImageRaster, Textures, 0);

    return layerImageRaster;
}

UOdysseyAnimationLayerImageRaster*
UOdysseyPainterEditorAnimationFunctionLibrary::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    if ( Paths.Num() <= 0 || !Animation || (ParentLayer && ParentLayer->GetAnimation() != Animation))
        return nullptr;

    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(Animation->GetLayerStack());
    if ( !layerStack )
        return nullptr;

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("animation-editor.transaction.import-image-sequence", "Import Image Sequence"));
    #endif
    UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), ParentLayer, IndexInParent));

    UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportImageSequence(layer, Paths, 0);

    return layer;
}

TArray<FString>
UOdysseyPainterEditorAnimationFunctionLibrary::ExportAsImageSequence(
    UOdysseyAnimation* Animation,
    FInt32Range FrameRange,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Animation)
        return {};

    FIntRect rect(0, 0, Animation->GetWidth(), Animation->GetHeight());
    return Odyssey::ExportAsImageSequence(Animation, FrameRange, rect, Filename, Path, Format );
}

FString
UOdysseyPainterEditorAnimationFunctionLibrary::ExportFrameAsImage(
    UOdysseyAnimation* Animation,
    int Frame,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Animation)
        return TEXT("");

    FIntRect rect(0, 0, Animation->GetWidth(), Animation->GetHeight());
    return Odyssey::ExportAsImage(Animation, Frame, Format, rect, Filename, Path );
}

UTexture2D*
UOdysseyPainterEditorAnimationFunctionLibrary::ExportFrameAsTexture(
    UOdysseyAnimation* Animation,
    int Frame,
    FString Filename,
    FString Path
)
{
    if (!Animation)
        return nullptr;

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(Animation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    FIntRect rect(0, 0, Animation->GetWidth(), Animation->GetHeight());
    return Odyssey::ExportAsTexture(Animation, Frame, rect, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyPainterEditorAnimationFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Animation )
        return {};

    return Odyssey::ExportAsTextureSequence(
        Animation,
        FrameRange,
        FIntRect(0, 0, Animation->GetWidth(), Animation->GetHeight()),
        AssetName,
        Path
    );
}

UPaperFlipbook*
UOdysseyPainterEditorAnimationFunctionLibrary::ExportAsFlipbook(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Animation )
        return nullptr;

    return Odyssey::ExportAsFlipbook(
        Animation,
        FrameRange,
        FIntRect(0, 0, Animation->GetWidth(), Animation->GetHeight()),
        Animation->FramesPerSecond,
        AssetName,
        Path
    );
}

TArray<UOdysseyAnimationCellImageRaster*>
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, int iCellIndex)
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
    FIntRect dstRect = Layer->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(dstRect.Width(), dstRect.Height());

    for( int i = 0; i < cells.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        UTexture2D* texture = Textures[i];
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
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex)
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

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    TArray<TSharedPtr<::ULIS::FBlock>> blocks;
    for (const FString& filename : Paths)
    {
        progressBar.EnterProgressFrame();
        FString path( FPaths::ConvertRelativePathToFull( filename ) );
        FString extension = FPaths::GetExtension(path, false);
        ::ULIS::eFileFormat exportImageFormat = ::ULIS::FileFormat_png;
        bool extensionFound = false;
        for( int i = 0; i <= ::ULIS::FileFormat_hdr; ++i )
        {
            if( extension == ::ULIS::kwImageFormat[i] )
            {
                exportImageFormat = static_cast< ::ULIS::eFileFormat >( i );
                extensionFound = true;
                break;
            }
        }

        if( !extensionFound )
            continue;

        TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>();
        std::string stdPath( TCHAR_TO_UTF8(*path) );
        ::ULIS::ulError error = ctx.XLoadBlockFromDisk(
              *block
            , stdPath
        );

        if (error != ULIS_NO_ERROR)
            continue;

        ctx.Finish();

        if (block->IsHollow())
            continue;

        if (block->Width() == animation->GetWidth() && block->Height() == animation->GetHeight() && block->Format() == format)
        {
            blocks.Add(block);
            continue;
        }

        //Need to convert the block before adding it to the layer
        TSharedPtr<::ULIS::FBlock> blockProxy = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), format);

        ::ULIS::FEvent eventConvert;
        ctx.ConvertFormat(
            *block
            , *blockProxy
            , ::ULIS::FRectI::Auto
            , ::ULIS::FVec2I( 0 )
            , ULIS::FSchedulePolicy::CacheEfficient
            , 0
            , nullptr
            , &eventConvert
        );

        ctx.Finish();

        blocks.Add(blockProxy);
    }

    if (blocks.IsEmpty())
        return {};

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("animation-editor.transaction.import-image-sequence", "Import Image Sequence"));
    #endif

    TArray<UOdysseyLayerCell*> cells = Layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), iCellIndex, blocks.Num());

    TArray<UOdysseyAnimationCellImageRaster*> rasterCells;
    for (int i = 0; i < cells.Num(); i++)
    {
        TSharedPtr<::ULIS::FBlock> block = blocks[i];
        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        rasterCells.Add(cell);

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
        rasterBlockMutator.Copy(block,{ invalidRect });
        rasterBlockMutator.Commit();
    }

    return rasterCells;
}

TArray<FString>
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportAsImageSequence(
    UOdysseyAnimationLayer* Layer,
    FInt32Range FrameRange,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Layer)
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();
    FIntRect rect(0, 0, animation->GetWidth(), animation->GetHeight());
    return Odyssey::ExportAsImageSequence(Layer, FrameRange, rect, Filename, Path, Format );
}

FString
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportFrameAsImage(
    UOdysseyAnimationLayer* Layer,
    int Frame,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Layer)
        return TEXT("");

    UOdysseyAnimation* animation = Layer->GetAnimation();
    FIntRect rect(0, 0, animation->GetWidth(), animation->GetHeight());
    return Odyssey::ExportAsImage(Layer, Frame, Format, rect, Filename, Path );
}

UTexture2D*
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportFrameAsTexture(
    UOdysseyAnimationLayer* Layer,
    int Frame,
    FString Filename,
    FString Path
)
{
    if (!Layer)
        return nullptr;

    UOdysseyAnimation* animation = Layer->GetAnimation();
    FIntRect rect(0, 0, animation->GetWidth(), animation->GetHeight());
    return Odyssey::ExportAsTexture(Layer, Frame, rect, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Layer )
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();
    return Odyssey::ExportAsTextureSequence(
        Layer,
        FrameRange,
        FIntRect(0, 0, animation->GetWidth(), animation->GetHeight()),
        AssetName,
        Path
    );
}

UPaperFlipbook*
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportAsFlipbook(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Layer )
        return nullptr;

    UOdysseyAnimation* animation = Layer->GetAnimation();
    return Odyssey::ExportAsFlipbook(
        Layer,
        FrameRange,
        FIntRect(0, 0, animation->GetWidth(), animation->GetHeight()),
        animation->FramesPerSecond,
        AssetName,
        Path
    );
}

#undef LOCTEXT_NAMESPACE
