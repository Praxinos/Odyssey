// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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


FOdysseyPainterEditorAnimationImportResult
UOdysseyPainterEditorAnimationFunctionLibrary::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, FOdysseyImportTexturesParameters Parameters)
{
    FOdysseyImportTexturesParameters params = Parameters;
    params.Init(Textures, Animation->GetWidth(), Animation->GetHeight());
    return FOdysseyPainterEditorAnimationImport::ImportTextureSequence( Animation, params );
}

FOdysseyPainterEditorAnimationImportResult
UOdysseyPainterEditorAnimationFunctionLibrary::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, FOdysseyImportTexturesParameters Parameters )
{
    FOdysseyImportTexturesParameters params = Parameters;
    params.Init(Paths, Animation->GetWidth(), Animation->GetHeight());
    return FOdysseyPainterEditorAnimationImport::ImportTextureSequence( Animation, params );
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

    return Odyssey::ExportAsImageSequence(Animation, FrameRange, Filename, Path, Format );
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

    return Odyssey::ExportAsImage(Animation, Frame, Format, Filename, Path );
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

    return Odyssey::ExportAsTexture(Animation, Frame, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyPainterEditorAnimationFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Animation )
        return {};

    return Odyssey::ExportAsTextureSequence(
        Animation,
        FrameRange,
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
        Animation->FramesPerSecond,
        AssetName,
        Path
    );
}

FOdysseyPainterEditorAnimationImportResult
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, FOdysseyImportTexturesParameters Parameters, int CellIndex)
{
    UOdysseyAnimation* animation = Layer->GetAnimation();
    if (!animation)
        return FOdysseyPainterEditorAnimationImportResult();

    FOdysseyImportTexturesParameters params = Parameters;
    params.Init(Textures, animation->GetWidth(), animation->GetHeight());
    return FOdysseyPainterEditorAnimationLayerImport::ImportTextureSequence( Layer, params, CellIndex );
}

FOdysseyPainterEditorAnimationImportResult
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, FOdysseyImportTexturesParameters Parameters, int CellIndex)
{
    UOdysseyAnimation* animation = Layer->GetAnimation();
    if (!animation)
        return FOdysseyPainterEditorAnimationImportResult();

    FOdysseyImportTexturesParameters params = Parameters;
    params.Init(Paths, animation->GetWidth(), animation->GetHeight());
    return FOdysseyPainterEditorAnimationLayerImport::ImportTextureSequence( Layer, params, CellIndex );
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

    return Odyssey::ExportAsImageSequence(Layer, FrameRange, Filename, Path, Format );
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

    return Odyssey::ExportAsImage(Layer, Frame, Format, Filename, Path );
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

    return Odyssey::ExportAsTexture(Layer, Frame, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyPainterEditorAnimationLayerFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Layer )
        return {};

    return Odyssey::ExportAsTextureSequence(
        Layer,
        FrameRange,
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
    if( !animation )
        return nullptr;

    return Odyssey::ExportAsFlipbook(
        Layer,
        FrameRange,
        animation->FramesPerSecond,
        AssetName,
        Path
    );
}

#undef LOCTEXT_NAMESPACE
