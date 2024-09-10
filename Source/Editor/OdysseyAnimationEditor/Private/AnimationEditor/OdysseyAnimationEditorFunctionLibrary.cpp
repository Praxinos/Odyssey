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
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "Widgets/SOdysseyAnimationExportImageSequenceDialog.h"
#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "OdysseyFlipbookWrapper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "OdysseyTextureFunctionLibrary.h"
#include "LayerStack/OdysseyTextureLayerImageRaster.h"

UOdysseyAnimation*
UOdysseyAnimationEditorFunctionLibrary::CreateAnimationAsset(FString AssetName, FString PackagePath, int Width, int Height, EOdysseyAnimationFormat Format, float FramesPerSecond)
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

void
UOdysseyAnimationEditorFunctionLibrary::ImportTextureSequence(UOdysseyAnimation* Animation, TArray<UTexture2D*> Textures)
{
    if ( Textures.Num() <= 0 || !Animation)
        return;

	UOdysseyLayerStack* layerStack = Animation->GetLayerStack();
    if ( !layerStack )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);
    
	layerImageRaster->Modify();
    
    FScopedSlowTask progressBar(Textures.Num(), LOCTEXT("timeline-tab.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

	UTexture2D* openedTexture = Cast<UTexture2D>(Textures[0]);
    TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(openedTexture, Animation->GetFormat()));    

    for( int i = 0; i < Textures.Num(); i++ )
    {
        progressBar.EnterProgressFrame();

		UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(layerImageRaster->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
		FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
		::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
		rasterBlockMutator.Copy(textureBlock, { invalidRect });
		rasterBlockMutator.Commit();
    }
    
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(Animation);
    currentFrameMutator.Set(0);
    currentFrameMutator.Commit();
}

void           
UOdysseyAnimationEditorFunctionLibrary::ImportImageSequence(UOdysseyAnimation* Animation, TArray<FString> Paths)
{
    if ( Paths.Num() <= 0 || !Animation)
        return;

	UOdysseyLayerStack* layerStack = Animation->GetLayerStack();
    if ( !layerStack )
        return;

    FScopedSlowTask progressBar(Paths.Num(), LOCTEXT("timeline-tab.import-image-sequence.progress-bar.title", "Importing Image Sequence"));
    progressBar.MakeDialog();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( Animation->GetFormat() );
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

        if (block->Width() == Animation->GetWidth() && block->Height() == Animation->GetHeight() && block->Format() == Animation->GetFormat())
        {
            blocks.Add(block);
            continue;
        }
        
        //Need to convert the block before adding it to the layer
        TSharedPtr<::ULIS::FBlock> blockProxy = MakeShared<::ULIS::FBlock>(Animation->GetWidth(), Animation->GetHeight(), Animation->GetFormat());

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
        return;

    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-tab.transaction.import-image-sequence", "Import Image Sequence"));
    #endif
	layerStack->Modify();
    UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
	layer->Modify();
    for (TSharedPtr<::ULIS::FBlock> block : blocks)
    {
		UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
		
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
		FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
		::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
		rasterBlockMutator.Copy(block,{ invalidRect });
		rasterBlockMutator.Commit();
    }

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(Animation);
    currentFrameMutator.Set(0);
    currentFrameMutator.Commit();
}

void           
UOdysseyAnimationEditorFunctionLibrary::ExportImageSequence(
	UOdysseyAnimation* Animation,
	FString Path,
	EOdysseyAnimationExportImageSequenceFormat Format,
	EOdysseyAnimationExportImageSequenceSource Source,
	EOdysseyAnimationExportImageSequenceRange Range,
	FInt32Range CustomRange,
	bool UniqueFramesOnly)
{
    FOdysseyAnimationImageSequenceExporter exporter(Animation);
	exporter.mFormat = Format;
	exporter.mSource = Source;
	exporter.mRange = Range;
	exporter.mCustomRange = CustomRange;
	exporter.mUniqueFramesOnly = UniqueFramesOnly;

	exporter.Export(Path);
}

void
UOdysseyAnimationEditorFunctionLibrary::ExportAsFlipbook(UOdysseyAnimation* Animation, FString AssetName, FString Path)
{
	if ( Path.IsEmpty() || !Animation)
        return;

	UOdysseyLayerStack* layerStack = Animation->GetLayerStack();
    if ( !layerStack )
        return;

    FInt32Range frameRange = Animation->GetFrameRange();
    int startFrame = frameRange.GetLowerBoundValue();
    int endFrame = frameRange.GetUpperBoundValue();
	FString endFrameStr = FString::FromInt(endFrame);
	int numZero = endFrameStr.Len();

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("timeline-tab.export-as-flipbook.progress-bar.title", "Export As Flipbook"));
    progressBar.MakeDialog();

    // Create flipbook asset
    FString assetPath = Path;
	if (!assetPath.EndsWith("/"))
		assetPath += "/";
    FString flipbookAssetName = AssetName;
    FString flipbookPackagePath = assetPath + flipbookAssetName;
    UPackage* flipbookPackage = CreatePackage(*flipbookPackagePath);
    UPaperFlipbook* flipbook = NewObject<UPaperFlipbook>(flipbookPackage, UPaperFlipbook::StaticClass(), FName(*flipbookAssetName), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | RF_Transactional);

    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(Animation->GetFormat());    

    ::ULIS::eFormat blockFormat = ULISFormatForTextureSourceFormat(textureSourceFormat);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(blockFormat);
    TArray<FGuid> lastRenderingComposition;

    FOdysseyFlipbookWrapper flipbookWrapper;
    flipbookWrapper.SetFlipbook(flipbook);

    int lastKeyFrameIndex = -1;
    int lastKeyFrameFirstFrame = -1;
    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = Animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        if (lastKeyFrameIndex >= 0)
            flipbookWrapper.SetKeyFrameLength(lastKeyFrameIndex, i - lastKeyFrameFirstFrame);
        
        //Render frame block
        TSharedPtr<IOdysseyImageRenderer> renderer = Animation->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, i);
        renderer->Init();

    	TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(Animation->GetWidth(), Animation->GetHeight(), blockFormat);

        FOdysseyImageRendererCopyParams params(block, { block->Rect() });
        renderer->Copy(params, {});
        ctx.Finish();

        int keyFrameIndex = flipbook->GetNumKeyFrames();

        FString numStr = FString::Format(TEXT("{0}"), {i});
        FString textureName = flipbookAssetName + TEXT("_Texture") + numStr;
        FString spriteName = flipbookAssetName + TEXT("_Sprite") + numStr;

        FOdysseyTextureConfiguration textureConfiguration;
        textureConfiguration.Width = Animation->GetWidth();
        textureConfiguration.Height = Animation->GetHeight();
        textureConfiguration.Format = EOdysseyTextureSourceFormat::kCustom;
        textureConfiguration.CustomFormat = textureSourceFormat;
        textureConfiguration.Name = FName(*textureName);
        
        //Create the keyframe
        flipbookWrapper.CreateEmptyKeyFrame(keyFrameIndex);
        lastKeyFrameIndex = keyFrameIndex;
        lastKeyFrameFirstFrame = i;

        //Create the sprite and add it to the keyframe
        UPaperSprite* sprite = flipbookWrapper.CreateSprite(spriteName);
        if (!sprite)
            continue;

        //Create the texture and add it to the keyframe
        UTexture2D* texture = flipbookWrapper.CreateTexture(textureConfiguration);
        if (!texture)
            continue;

        flipbookWrapper.SetSpriteTexture(sprite, texture); //Finishes the sprite initialization before giving it to the flipbook, otherwise it calls some unwanted callbacks in the GUI
        flipbookWrapper.SetKeyframeSprite(keyFrameIndex, sprite);

		UOdysseyLayerStack* layerstack = UOdysseyTextureFunctionLibrary::GetLayerStack(texture);
		UOdysseyTextureLayerImageRaster* layer = Cast<UOdysseyTextureLayerImageRaster>(layerstack->GetLayers()[0]);

		TSharedPtr<FOdysseyRasterBlock> rasterBlock = layer->GetRasterBlock();
		FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
		::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, Animation->GetWidth(), Animation->GetHeight());
		rasterBlockMutator.Copy(block, { invalidRect });
		rasterBlockMutator.Commit();

        texture->PostEditChange();
        texture->UpdateResource();
    }

    if (lastKeyFrameIndex >= 0)
        flipbookWrapper.SetKeyFrameLength(lastKeyFrameIndex, endFrame - lastKeyFrameFirstFrame + 1);

    //Configure flipbook asset
    UClass* flipbookClass = flipbook->StaticClass();
    FObjectProperty* defaultMaterialProperty = FindFProperty<FObjectProperty>(flipbookClass, "DefaultMaterial");
    defaultMaterialProperty->SetObjectPropertyValue(defaultMaterialProperty->ContainerPtrToValuePtr<UPaperFlipbook>(flipbook), LoadObject<UMaterialInterface>(nullptr, TEXT("/Iliad/Animation2D/DefaultFlipbookMaterialInstance.DefaultFlipbookMaterialInstance")));

    FScopedFlipbookMutator mutator(flipbook);
	mutator.FramesPerSecond = Animation->FramesPerSecond;

    FAssetRegistryModule::AssetCreated(flipbook);

    FSavePackageArgs packageArgs;
    packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    UPackage::SavePackage( flipbookPackage, flipbook, *flipbookAssetName, packageArgs );
        
    flipbookPackage->MarkAsFullyLoaded();
    flipbook->MarkPackageDirty();
}