// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyTextureEditorFunctionLibrary.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"

#include "OdysseyExportImage.h"
#include "OdysseyLayerStack.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyRasterBlockMutator.h"
#include "OdysseyTextureFactory.h"
#include "OdysseyTextureFunctionLibrary.h"
#include "OdysseyTextureLayer.h"
#include "OdysseyTextureLayerImageRaster.h"
#include "OdysseyTextureLayerStack.h"

#include "ULISLoaderModule.h"

UOdysseyTextureFactory*
UOdysseyTextureEditorTextureFunctionLibrary::GetTextureFactory()
{
    static UOdysseyTextureFactory* textureFactory = nullptr;
    if (!textureFactory)
    {
        textureFactory = NewObject<UOdysseyTextureFactory>();
        textureFactory->AddToRoot();
    }
    return textureFactory;
}

UTexture2D*
UOdysseyTextureEditorTextureFunctionLibrary::CreateTextureAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyTextureSourceFormat Format)
{
    if (AssetName.IsEmpty())
        return nullptr;

    if (PackagePath.IsEmpty())
        return nullptr;

    if (Width <= 0 || Height <= 0)
        return nullptr;

    FOdysseyTextureConfiguration configuration;
    configuration.Name = FName(*AssetName);
    configuration.Width = Width;
    configuration.Height = Height;
    configuration.Format = Format;
    configuration.LayerType = EOdysseyTextureDefaultLayerType::kNone;

    GetTextureFactory()->SetConfiguration(configuration);

    IAssetTools& assetTools = FAssetToolsModule::GetModule().Get();
    UTexture2D* Texture = Cast<UTexture2D>(
        assetTools.CreateAsset(
            AssetName,
            PackagePath,
            UTexture2D::StaticClass(),
            GetTextureFactory()
        )
    );

    return Texture;
}

UOdysseyTextureLayerImageRaster*
UOdysseyTextureEditorTextureFunctionLibrary::ImportTexture(UTexture2D* Texture, UTexture2D* TextureToImport, UOdysseyTextureLayer* ParentLayer, int IndexInLayer)
{
    if ( !Texture || !TextureToImport || (ParentLayer && ParentLayer->GetTexture() != Texture) )
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = UOdysseyTextureFunctionLibrary::GetLayerStack(Texture);
    if ( !layerStack )
        return nullptr;

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), ParentLayer, IndexInLayer);
    if (!layer)
        return nullptr;

    UOdysseyTextureLayerImageRaster* layerImageRaster = Cast<UOdysseyTextureLayerImageRaster>(layer);
    FOdysseyRasterContent& content = layerImageRaster->GetRasterContent();
    TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(TextureToImport, content.Describe().Format));
    content.WriteBlock(*textureBlock, { FIntRect(0, 0, textureBlock->Width(), textureBlock->Height()) });

    return layerImageRaster;
}

UOdysseyTextureLayerImageRaster*
UOdysseyTextureEditorTextureFunctionLibrary::ImportImage(UTexture2D* Texture, FString Path, UOdysseyTextureLayer* ParentLayer, int IndexInLayer)
{
    if ( Path.IsEmpty() || !Texture || (ParentLayer && ParentLayer->GetTexture() != Texture))
        return nullptr;

    UOdysseyTextureLayerStack* layerStack = UOdysseyTextureFunctionLibrary::GetLayerStack(Texture);
    if ( !layerStack )
        return nullptr;

    UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->AddLayer(UOdysseyTextureLayerImageRaster::StaticClass(), ParentLayer, IndexInLayer));
    if (!layer)
        return nullptr;

    FOdysseyRasterContent& content = layer->GetRasterContent();
    const FOdysseyRasterContent::FDescription description = content.Describe();
    ::ULIS::eFormat format = description.Format;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    FString path( FPaths::ConvertRelativePathToFull( Path ) );
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
        return nullptr;

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>();
    std::string stdPath( TCHAR_TO_UTF8(*path) );
    ::ULIS::ulError error = ctx.XLoadBlockFromDisk(
            *block
        , stdPath
    );

    if (error != ULIS_NO_ERROR)
        return nullptr;

    ctx.Finish();

    if (block->IsHollow())
        return nullptr;

    if (block->Width() != description.Size.X || block->Height() != description.Size.Y || block->Format() != description.Format)
    {

        //Need to convert the block before adding it to the layer
        TSharedPtr<::ULIS::FBlock> blockProxy = MakeShared<::ULIS::FBlock>(description.Size.X, description.Size.Y, description.Format);

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

        block = blockProxy;
    }

    content.WriteBlock(*block, { FIntRect(0, 0, block->Width(), block->Height()) });

    return layer;
}

FString
UOdysseyTextureEditorTextureFunctionLibrary::ExportAsImage(
    UTexture2D* Texture,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Texture)
        return TEXT("");

    UOdysseyTextureLayerStack* layerStack = UOdysseyTextureFunctionLibrary::GetLayerStack(Texture);
    if ( !layerStack )
        return TEXT("");

    return Odyssey::ExportAsImage(layerStack, nullptr, Format, Filename, Path );
}

FString
UOdysseyTextureEditorLayerFunctionLibrary::ExportAsImage(
    UOdysseyTextureLayer* Layer,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if (!Layer)
        return TEXT("");

    UTexture2D* texture = Layer->GetTexture();
    return Odyssey::ExportAsImage(Layer, nullptr, Format, Filename, Path );
}

UTexture2D*
UOdysseyTextureEditorLayerFunctionLibrary::ExportAsTexture(
    UOdysseyTextureLayer* Layer,
    FString Filename,
    FString Path
)
{
    if (!Layer)
        return nullptr;

    UTexture2D* texture = Layer->GetTexture();
    return Odyssey::ExportAsTexture(Layer, nullptr, Filename, Path );
}
