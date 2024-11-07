// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationEditorFunctionLibrary.h"
#include "OdysseyLayerStack.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "OdysseyAnimationFactory.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Widgets/SOdysseyAnimationExportImageSequenceDialog.h"
#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyFlipbookWrapper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "OdysseyTextureFunctionLibrary.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "ScopedTransaction.h"
#include "OdysseyRasterBlockMutator.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "UOdysseyAnimationEditorAnimationFunctionLibrary"

UOdysseyAnimation*
UOdysseyAnimationEditorAnimationFunctionLibrary::CreateAnimationAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyAnimationFormat Format, float FramesPerSecond)
{
    if (AssetName.IsEmpty())
        return nullptr;

    if (PackagePath.IsEmpty())
        return nullptr;

    if (Width <= 0 || Height <= 0)
        return nullptr;

    if (FramesPerSecond < 0.f)
        return nullptr;

    IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
    UOdysseyAnimation* animation = Cast<UOdysseyAnimation>(
        assetTools.CreateAsset(
            AssetName,
            PackagePath,
            UOdysseyAnimation::StaticClass(),
            UOdysseyAnimationFactory::StaticClass()->GetDefaultObject<UFactory>()
        )
    );

    if (!animation)
        return nullptr;

    animation->mWidth = Width;
    animation->mHeight = Height;
    animation->Format = Format;
    animation->FramesPerSecond = FramesPerSecond;

    return animation;
}

UOdysseyAnimationLayerImageRaster*
UOdysseyAnimationEditorAnimationFunctionLibrary::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    if ( Textures.Num() <= 0 || !Animation || (ParentLayer && ParentLayer->GetAnimation() != Animation))
        return nullptr;

    UOdysseyLayerStack* layerStack = Animation->GetLayerStack();
    if ( !layerStack )
        return nullptr;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), ParentLayer, IndexInParent);
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);
    
    FScopedSlowTask progressBar(Textures.Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    UOdysseyAnimationEditorLayerFunctionLibrary::ImportTextureSequence(layerImageRaster, Textures, 0);

    return layerImageRaster;
}

UOdysseyAnimationLayerImageRaster*
UOdysseyAnimationEditorAnimationFunctionLibrary::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths, UOdysseyAnimationLayer* ParentLayer, int IndexInParent)
{
    if ( Paths.Num() <= 0 || !Animation || (ParentLayer && ParentLayer->GetAnimation() != Animation))
        return nullptr;

    UOdysseyLayerStack* layerStack = Animation->GetLayerStack();
    if ( !layerStack )
        return nullptr;

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("animation-editor.transaction.import-image-sequence", "Import Image Sequence"));
    #endif
    UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), ParentLayer, IndexInParent));
    
    UOdysseyAnimationEditorLayerFunctionLibrary::ImportImageSequence(layer, Paths, 0);

    return layer;
}

TArray<FString>
UOdysseyAnimationEditorAnimationFunctionLibrary::ExportAsImageSequence(
    UOdysseyAnimation* Animation,
    FInt32Range FrameRange,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Animation)
        return {};

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
    return Animation->ExportAsImageSequence(Animation->GetFormat(), FrameRange, rect, Filename, Path, Format );
}

FString
UOdysseyAnimationEditorAnimationFunctionLibrary::ExportFrameAsImage(
    UOdysseyAnimation* Animation,
    int Frame,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Animation)
        return TEXT("");

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
    return Animation->ExportAsImage(Animation->GetFormat(), Frame, Format, rect, Filename, Path );
}

UTexture2D*
UOdysseyAnimationEditorAnimationFunctionLibrary::ExportFrameAsTexture(
    UOdysseyAnimation* Animation,
    int Frame,
    FString Filename,
    FString Path
)
{
    if (!Animation)
        return nullptr;

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(Animation->GetFormat());
    return Animation->ExportAsTexture(Frame, rect, textureSourceFormat, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyAnimationEditorAnimationFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Animation )
        return {};

    return Animation->ExportAsTextureSequence(
        Animation->GetFormat(),
        FrameRange,
        ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight()),
        AssetName,
        Path
    );
}

UPaperFlipbook*
UOdysseyAnimationEditorAnimationFunctionLibrary::ExportAsFlipbook(UOdysseyAnimation* Animation, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Animation )
        return nullptr;

    return Animation->ExportAsFlipbook(
        Animation->GetFormat(),
        FrameRange,
        ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight()),
        Animation->FramesPerSecond,
        AssetName,
        Path
    );
}

TArray<UOdysseyAnimationCellImageRaster*>
UOdysseyAnimationEditorLayerFunctionLibrary::ImportTextureSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<UTexture2D*> Textures, int iCellIndex)
{
    if ( Textures.Num() <= 0 || !Layer)
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();

    if (iCellIndex != INDEX_NONE)
        iCellIndex = FMath::Clamp(iCellIndex, 0, Layer->GetCells().Num());

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    
    FScopedSlowTask progressBar(Textures.Num(), LOCTEXT("animation-editor.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    UTexture2D* openedTexture = Cast<UTexture2D>(Textures[0]);
    TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(openedTexture, animation->GetFormat()));

    TArray<UOdysseyAnimationCell*> cells = Layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), iCellIndex, Textures.Num());
    TArray<UOdysseyAnimationCellImageRaster*> rasterCells;
    for( int i = 0; i < cells.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

        UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(cells[i]);
        UTexture2D* texture = Textures[i];

        rasterCells.Add(cell);

        FillOdysseyBlockFromUTextureData(textureBlock.Get(), texture, animation->GetFormat());

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
        FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
        ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
        rasterBlockMutator.Copy(textureBlock, { invalidRect });
        rasterBlockMutator.Commit();
    }

    return rasterCells;
}

TArray<UOdysseyAnimationCellImageRaster*>
UOdysseyAnimationEditorLayerFunctionLibrary::ImportImageSequence(UOdysseyAnimationLayerImageRaster* Layer, TArray<FString> Paths, int iCellIndex)
{
    if ( Paths.Num() <= 0 || !Layer)
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();

    if (iCellIndex != INDEX_NONE)
        iCellIndex = FMath::Clamp(iCellIndex, 0, Layer->GetCells().Num());

    FScopedSlowTask progressBar(Paths.Num(), LOCTEXT("animation-editor.import-image-sequence.progress-bar.title", "Importing Image Sequence"));
    progressBar.MakeDialog();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( animation->GetFormat() );
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

        if (block->Width() == animation->GetWidth() && block->Height() == animation->GetHeight() && block->Format() == animation->GetFormat())
        {
            blocks.Add(block);
            continue;
        }
        
        //Need to convert the block before adding it to the layer
        TSharedPtr<::ULIS::FBlock> blockProxy = MakeShared<::ULIS::FBlock>(animation->GetWidth(), animation->GetHeight(), animation->GetFormat());

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

    TArray<UOdysseyAnimationCell*> cells = Layer->AddCells(UOdysseyAnimationCellImageRaster::StaticClass(), iCellIndex, blocks.Num());

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
UOdysseyAnimationEditorLayerFunctionLibrary::ExportAsImageSequence(
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

    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
    return Layer->ExportAsImageSequence(animation->GetFormat(), FrameRange, rect, Filename, Path, Format );
}

FString
UOdysseyAnimationEditorLayerFunctionLibrary::ExportFrameAsImage(
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
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
    return Layer->ExportAsImage(animation->GetFormat(), Frame, Format, rect, Filename, Path );
}

UTexture2D*
UOdysseyAnimationEditorLayerFunctionLibrary::ExportFrameAsTexture(
    UOdysseyAnimationLayer* Layer,
    int Frame,
    FString Filename,
    FString Path
)
{
    if (!Layer)
        return nullptr;

    UOdysseyAnimation* animation = Layer->GetAnimation();
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight());
    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(animation->GetFormat());
    return Layer->ExportAsTexture(Frame, rect, textureSourceFormat, Filename, Path );
}

TArray<UTexture2D*>
UOdysseyAnimationEditorLayerFunctionLibrary::ExportAsTextureSequence(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Layer )
        return {};

    UOdysseyAnimation* animation = Layer->GetAnimation();

    return Layer->ExportAsTextureSequence(
        animation->GetFormat(),
        FrameRange,
        ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight()),
        AssetName,
        Path
    );
}

UPaperFlipbook*
UOdysseyAnimationEditorLayerFunctionLibrary::ExportAsFlipbook(UOdysseyAnimationLayer* Layer, FInt32Range FrameRange, FString AssetName, FString Path)
{
    if ( !Layer )
        return nullptr;

    UOdysseyAnimation* animation = Layer->GetAnimation();

    return Layer->ExportAsFlipbook(
        animation->GetFormat(),
        FrameRange,
        ::ULIS::FRectI::FromXYWH(0, 0, animation->GetWidth(), animation->GetHeight()),
        animation->FramesPerSecond,
        AssetName,
        Path
    );
}

#undef LOCTEXT_NAMESPACE
