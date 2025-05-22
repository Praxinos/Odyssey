// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorAnimationFunctionLibrary.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "Animation/OdysseyPainterEditorAnimationImport.h"
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
#include "ScreenPass.h"
#include "ImageUtils.h"
#include "Engine/TextureRenderTarget2D.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorAnimationFunctionLibrary"


UOdysseyAnimationLayerImageRaster*
UOdysseyPainterEditorAnimationFunctionLibrary::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    FOdysseyPainterEditorAnimationImport import_animation;
    return import_animation.ImportTextureSequence( Animation, Textures, ParentLayer, IndexInParent );
}

UOdysseyAnimationLayerImageRaster*
UOdysseyPainterEditorAnimationFunctionLibrary::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    FOdysseyPainterEditorAnimationImport import_animation;
    return import_animation.ImportImageSequence( Animation, Paths, ParentLayer, IndexInParent );
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
    FOdysseyPainterEditorAnimationLayerImport import_layer;
    return import_layer.ImportTextureSequence( Layer, Textures, iCellIndex );
}

TArray<UOdysseyAnimationCellImageRaster*>
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex)
{
    FOdysseyPainterEditorAnimationLayerImport import_layer;
    return import_layer.ImportImageSequence( Layer, Paths, iCellIndex );
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
