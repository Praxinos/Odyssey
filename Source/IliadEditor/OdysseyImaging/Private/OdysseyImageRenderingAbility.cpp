// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyImageRenderingAbility.h"

#include "AssetToolsModule.h"
#include "Factories/Texture2dFactoryNew.h"
#include "HAL/PlatformFileManager.h"
#include "Materials/MaterialInterface.h"
#include "OdysseyPixelFormat.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "PaperFlipbookFactory.h"
#include "PaperSpriteFactory.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "ULISLoaderModule.h"

#define LOCTEXT_NAMESPACE "Imaging"

FOdysseyImageRenderingChangedEvent::FOdysseyImageRenderingChangedEvent(eEventType iType, bool iIsInteractive, const FGuid& iId, const TArray<::ULIS::FRectI>& iRects)
    : mType(iType)
    , mIsInteractive(iIsInteractive)
    , mId(iId)
    , mRects(iRects)
{
}

FOdysseyImageRenderingChangedEvent::eEventType
FOdysseyImageRenderingChangedEvent::GetType() const
{
    return mType;
}

bool
FOdysseyImageRenderingChangedEvent::IsInteractive() const
{
    return mIsInteractive;
}

const FGuid&
FOdysseyImageRenderingChangedEvent::GetId() const
{
    return mId;
}

const TArray<::ULIS::FRectI>&
FOdysseyImageRenderingChangedEvent::GetRects() const
{
    return mRects;
}

FOdysseyImageRenderingAbility::FOnChanged&
FOdysseyImageRenderingAbility::OnImageRenderingPreChangedDelegate()
{
    static FOnChanged onPreChanged;
    return onPreChanged;
}

FOdysseyImageRenderingAbility::FOnChanged&
FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate()
{
    static FOnChanged onChanged;
    return onChanged;
}

FOdysseyImageRenderingAbility::FOdysseyImageRenderingAbility()
    : mImageRenderingId(FGuid::NewGuid())
{

}

void
FOdysseyImageRenderingAbility::ImageRenderingChanged(bool iIsInteractive)
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetImageRenderingId(), GetImageRenderingRects());
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kValueChange, iIsInteractive, GetImageRenderingId(), iRects);
    OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    OnImageRenderingChangedDelegate().Broadcast(eventChanged);
}

void
FOdysseyImageRenderingAbility::ImageRenderingCompositionChanged(bool iIsInteractive)
{
    FOdysseyImageRenderingChangedEvent eventChanged(FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange, iIsInteractive, GetImageRenderingId(), {});
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyImageRenderingAbility::ImageRenderingCompositionChanged::PreChange);
        OnImageRenderingPreChangedDelegate().Broadcast(eventChanged);
    }

    {
        TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyImageRenderingAbility::ImageRenderingCompositionChanged::Change);
        OnImageRenderingChangedDelegate().Broadcast(eventChanged);
    }
}

TArray<::ULIS::FRectI>
FOdysseyImageRenderingAbility::GetImageRenderingRects() const
{
    check(false); //If you need it, override it in your class
    return {};
}

FGuid
FOdysseyImageRenderingAbility::GetImageRenderingId() const
{
    return mImageRenderingId;
}

TSharedPtr<IOdysseyImageRenderer>
FOdysseyImageRenderingAbility::BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter) const
{
    check(false); //If you need it, override it in your class
    return nullptr;
}

TArray<FGuid>
FOdysseyImageRenderingAbility::GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const
{
    check(false); //If you need it, override it in your class
    return {};
}

UTexture2D*
FOdysseyImageRenderingAbility::ExportAsTexture(int iFrame, const ::ULIS::FRectI& iRect, ETextureSourceFormat iFormat, FString iAssetName, FString iPath )
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    ::ULIS::eFormat blockFormat = ULISFormatForTextureSourceFormat(iFormat);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(blockFormat);

    //Render frame block
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, blockFormat);

    TSharedPtr<IOdysseyImageRenderer> renderer = BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, iFrame);
    renderer->Init();

    FOdysseyImageRendererCopyParams params(block, { block->Rect() }, iRect.Position());
    renderer->Copy(params, {});
    ctx.Finish();

    //Create the texture and add it to the keyframe
    UTexture2D* texture2D = Cast<UTexture2D>( AssetTools.CreateAsset(
        iAssetName,
        iPath,
        UTexture2D::StaticClass(),
        UTexture2DFactoryNew::StaticClass()->GetDefaultObject<UFactory>()
    ));

    InitTextureWithBlockData(block.Get(), texture2D, texture2D->Source.GetFormat());

    texture2D->PostEditChange();
    texture2D->UpdateResource();
    texture2D->MarkPackageDirty();

    return texture2D;
}

FString
FOdysseyImageRenderingAbility::ExportAsImage(::ULIS::eFormat iULISFormat, int iFrame, EOdysseyExportImageFormat iFormat, const ::ULIS::FRectI& iRect, FString iFilename, FString iPath)
{
    ::ULIS::eFormat blockFormat = iULISFormat;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(blockFormat);

    TSharedPtr<IOdysseyImageRenderer> renderer = BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, iFrame);
    renderer->Init();

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(iRect.w, iRect.h, blockFormat);
    FOdysseyImageRendererCopyParams params(block, { block->Rect() }, iRect.Position());
    renderer->Copy(params, {});
    ctx.Finish();

    ::ULIS::eFileFormat fileFormat = FOdysseyExportImageFormat::GetFileFormat(iFormat);
    FString extension = FOdysseyExportImageFormat::GetFileFormatExtension(iFormat);

    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();

    // Ensure Directory Exists
    if (!platformFile.DirectoryExists(*iPath))
        platformFile.CreateDirectory(*iPath);

    //Path
    FString imagePath = iPath / FPaths::GetBaseFilename(iFilename) + TEXT(".") + extension;

    std::string str = std::string( TCHAR_TO_UTF8( *imagePath ) );

    bool canSaveDirectly = false;
    ::ULIS::FContext::SaveBlockToDiskMetrics( *block, fileFormat, &canSaveDirectly );
    if (canSaveDirectly)
    {
        ctx.SaveBlockToDisk(
            *block
            , str
            , fileFormat
            , 100
        );

        ctx.Finish();
    }
    else
    {
        ::ULIS::eFormat format = block->Model() == ::ULIS::ColorModel_GREY ? ::ULIS::Format_GA8 : ::ULIS::Format_RGBA8;
        if (fileFormat == ::ULIS::FileFormat_hdr)
        {
            format = ::ULIS::Format_RGBAF;
        }

        ::ULIS::FBlock blockProxy(block->Width(), block->Height(), format);

        ::ULIS::FEvent eventConvert;
        ctx.ConvertFormat(
            *block
            , blockProxy
            , ::ULIS::FRectI::Auto
            , ::ULIS::FVec2I( 0 )
            , ULIS::FSchedulePolicy::CacheEfficient
            , 0
            , nullptr
            , &eventConvert
        );

        ctx.SaveBlockToDisk(
            blockProxy
            , str
            , fileFormat
            , 100
        );

        ctx.Finish();
    }
    return imagePath;
}

UPaperFlipbook*
FOdysseyImageRenderingAbility::ExportAsFlipbook(::ULIS::eFormat iULISFormat, const FInt32Range& iRange, const ::ULIS::FRectI& iRect, float iFramesPerSecond, FString AssetName, FString Path)
{
    if ( Path.IsEmpty())
        return nullptr;

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();
    FString endFrameStr = FString::FromInt(endFrame);

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-flipbook.progress-bar.title", "Export As Flipbook"));
    progressBar.MakeDialog();

    // Create flipbook asset
    FString flipbookAssetName = AssetName;
    FString flipbookPackagePath = Path;

    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(AssetTools.CreateAsset(
        flipbookAssetName,
        flipbookPackagePath,
        UPaperFlipbook::StaticClass(),
        UPaperFlipbookFactory::StaticClass()->GetDefaultObject<UFactory>()
    ));

    //sprite factory is not easily accessible because not exported through PAPER2D_API
    UFactory* spriteFactory = *AssetTools.GetNewAssetFactories().FindByPredicate(
        [](UFactory* iFactory)
        {
            return iFactory->DoesSupportClass(UPaperSprite::StaticClass());
        }
    );

    FScopedFlipbookMutator flipbookMutator(flipbook);
    flipbookMutator.FramesPerSecond = iFramesPerSecond;

    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(iULISFormat);
    TArray<FGuid> lastRenderingComposition;

    int lastKeyFrameIndex = -1;
    int lastKeyFrameFirstFrame = -1;
    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        if (!flipbookMutator.KeyFrames.IsEmpty())
            flipbookMutator.KeyFrames.Last().FrameRun = i - lastKeyFrameFirstFrame;

        FString numStr = FString::Format(TEXT("{0}"), {i});
        FString textureName = flipbookAssetName + TEXT("_Texture_") + numStr;
        FString spriteName = flipbookAssetName + TEXT("_Sprite_") + numStr;

        lastKeyFrameFirstFrame = i;

        UPaperSprite* sprite = Cast<UPaperSprite>(AssetTools.CreateAsset(
            spriteName,
            flipbookPackagePath,
            UPaperSprite::StaticClass(),
            spriteFactory
        ));

        UTexture2D* texture = ExportAsTexture(i, iRect, textureSourceFormat, textureName, flipbookPackagePath);
        FOdysseyObjectEditorUtils::SetPropertyValue(sprite, "SourceTexture", TSoftObjectPtr<UTexture2D>(texture));

        FPaperFlipbookKeyFrame keyframe;
        keyframe.Sprite = sprite;
        flipbookMutator.KeyFrames.Add(keyframe);
    }

    if (!flipbookMutator.KeyFrames.IsEmpty())
            flipbookMutator.KeyFrames.Last().FrameRun = endFrame - lastKeyFrameFirstFrame + 1;

    //Configure flipbook asset
    UMaterialInterface* material = LoadObject<UMaterialInterface>(nullptr, TEXT("/Odyssey/Animation2D/DefaultFlipbookMaterialInstance.DefaultFlipbookMaterialInstance"));
    FOdysseyObjectEditorUtils::SetPropertyValue(flipbook, "DefaultMaterial", material);

    return flipbook;
}

TArray<UTexture2D*>
FOdysseyImageRenderingAbility::ExportAsTextureSequence(::ULIS::eFormat iULISFormat, const FInt32Range& iRange, const ::ULIS::FRectI& iRect, FString AssetName, FString Path)
{
    if ( Path.IsEmpty())
        return {};

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();
    FString endFrameStr = FString::FromInt(endFrame);

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-texture-sequence.progress-bar.title", "Export As Texture Sequence"));
    progressBar.MakeDialog();

    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(iULISFormat);
    TArray<FGuid> lastRenderingComposition;

    TArray<UTexture2D*> textures;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;
        FString numStr = FString::Format(TEXT("{0}"), {i});
        FString textureName = AssetName + TEXT("_") + numStr;

        UTexture2D* texture = ExportAsTexture(i, iRect, textureSourceFormat, textureName, Path);

        textures.Add(texture);
    }

    return textures;
}

TArray<FString>
FOdysseyImageRenderingAbility::ExportAsImageSequence(
    ::ULIS::eFormat iULISFormat,
    const FInt32Range& iRange,
    const ::ULIS::FRectI& iRect,
    FString Filename,
    FString Path,
    EOdysseyExportImageFormat Format
)
{
    if ( Path.IsEmpty())
        return {};

    int startFrame = iRange.GetLowerBoundValue();
    int endFrame = iRange.GetUpperBoundValue();

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("image-rendering-ability.export-as-texture-sequence.progress-bar.title", "Export As Texture Sequence"));
    progressBar.MakeDialog();

    TArray<FGuid> lastRenderingComposition;

    FString endFrameStr = FString::FromInt(endFrame);
    TArray<FString> paths;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        FString frameStr = FString::FromInt(i);
        FString filename = Filename + TEXT("_");
         for (int j = 0; j < endFrameStr.Len() - frameStr.Len(); j++)
        {
            filename += TEXT("0");
        }
        filename += FString::Printf(TEXT("%d"), i);

        FString fullpath = ExportAsImage(iULISFormat, i, Format, iRect, filename, Path );

        paths.Add(fullpath);
    }

    return paths;
}

#undef LOCTEXT_NAMESPACE
