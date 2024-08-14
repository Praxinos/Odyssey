// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Misc/ScopedSlowTask.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "DesktopPlatformModule.h"

#include "PaperFlipbook.h"
#include "OdysseyPixelFormat.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"
#include "OdysseyFlipbookWrapper.h"
#include "Widgets/SOdysseyAnimationExportImageSequenceDialog.h"
#include "AnimationEditor/OdysseyAnimationEditorCommands.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

const FName&
FOdysseyAnimationEditorTimelineTab::StaticId()
{
    static FName Id = TEXT("OdysseyAnimationEditor_LayerStack"); //Keep this name
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorTimelineTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorTimelineTab::~FOdysseyAnimationEditorTimelineTab()
{
}

FOdysseyAnimationEditorTimelineTab::FOdysseyAnimationEditorTimelineTab(FOdysseyAnimationEditorExtension* iExtension)
	: FOdysseyEditorTab(LOCTEXT( "timeline-tab.name", "Timeline" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mExtension(iExtension)
    , mEmptyTimelineTabWidget(CreateDefaultEmptyTimelineTabWidget())
{
}

void
FOdysseyAnimationEditorTimelineTab::SetEmptyTimelineWidget(TSharedRef<SWidget> iWidget)
{
    mEmptyTimelineTabWidget = iWidget;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

const FName&
FOdysseyAnimationEditorTimelineTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyAnimationEditorTimelineTab::CreateWidget()
{
    
    return 
        SNew(SWidgetSwitcher)
        .WidgetIndex_Lambda([this](){ return LayerStack() == nullptr ? 1 : 0; })
        +SWidgetSwitcher::Slot()
        [
            SNew(SOdysseyAnimationLayerStack, mExtension)
            .LayerStack(this, &FOdysseyAnimationEditorTimelineTab::LayerStack)
        ]
        +SWidgetSwitcher::Slot()
        [
            //Display a PlaceHolder when no layerstack can be displayed
            mEmptyTimelineTabWidget.ToSharedRef()
        ];
}

TSharedPtr<SWidget>
FOdysseyAnimationEditorTimelineTab::CreateDefaultEmptyTimelineTabWidget() const
{
    return SNew(STextBlock)
        .Text(LOCTEXT("timeline-tab.nothing-to-display", "No Timeline can be displayed"));
}

void
FOdysseyAnimationEditorTimelineTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyAnimationEditorCommands& AnimationEditorCommands = FOdysseyAnimationEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyAnimationEditorTimelineTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(AnimationEditorCommands.ImportTextureSequence, ImportTextureSequence )
    MAP_ACTION(AnimationEditorCommands.ImportImageSequence, ImportImageSequence )
    MAP_ACTION(AnimationEditorCommands.ExportImageSequence, ExportImageSequence )
    MAP_ACTION(AnimationEditorCommands.ExportAsFlipbook, ExportAsFlipbook )
    MAP_ACTION(AnimationEditorCommands.CreateNewAnimationLayerImageRaster, CreateNewLayer )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity10, ChangeLayerOpacity, 0.1f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity20, ChangeLayerOpacity, 0.2f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity30, ChangeLayerOpacity, 0.3f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity40, ChangeLayerOpacity, 0.4f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity50, ChangeLayerOpacity, 0.5f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity60, ChangeLayerOpacity, 0.6f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity70, ChangeLayerOpacity, 0.7f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity80, ChangeLayerOpacity, 0.8f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity90, ChangeLayerOpacity, 0.9f )
    MAP_ACTION(AnimationEditorCommands.ChangeLayerOpacity100, ChangeLayerOpacity, 1.0f )

    #undef MAP_ACTION
}

void
FOdysseyAnimationEditorTimelineTab::ExtendMenu(FToolMenuOwner iOwner, FName iMenuName)
{
    ExtendMenuFile(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditorTimelineTab::LayerStack() const
{
    return mExtension->LayerStack();
}

UOdysseyAnimation*
FOdysseyAnimationEditorTimelineTab::Animation() const
{
    return mExtension->Animation();
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditorTimelineTab::Player() const
{
    return mExtension->Player();
}

float
FOdysseyAnimationEditorTimelineTab::PlaybackFramesPerSecond() const
{
    return mExtension->PlaybackFramesPerSecond();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyAnimationEditorTimelineTab::ExtendMenuFile( FToolMenuOwner iOwner, FName iMenuName )
{
    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".File")));

    FToolMenuInsert menuInsert;
    if (menu->FindSection("FileActors")) //FileActirs is a weird name but it is the actual name of the "Import/Export" Section from Unreal File Menu
        menuInsert = FToolMenuInsert("FileActors", EToolMenuInsertType::After);

    menu->AddDynamicSection(
        "OdysseyAnimationDynamic",
        FNewToolMenuDelegate::CreateLambda(
            [this](UToolMenu* iToolMenu)
            {
                FOdysseyPainterEditor* editor = mExtension->GetEditor();
                if (!editor)
                    return;

                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
                    return;
                
                FToolMenuSection& section = iToolMenu->AddSection("OdysseyAnimation", LOCTEXT("timeline-tab.file-menu.animation-import-export-section.name", "Animation Import / Export"));
                {
                    section.AddSubMenu(
                        TEXT("Import"),
                        LOCTEXT("timeline-tab.file-menu.import-submenu.name", "Import"),
                        LOCTEXT("timeline-tab.file-menu.import-submenu.tooltip", "Contains Import actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyAnimationEditorTimelineTab::BuildImportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Import" )
                    );

                    section.AddSubMenu(
                        TEXT("Export"),
                        LOCTEXT("timeline-tab.file-menu.export-submenu.name", "Export"),
                        LOCTEXT("timeline-tab.file-menu.export-submenu.tooltip", "Contains Export actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyAnimationEditorTimelineTab::BuildExportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Export" )
                    );
                }
            }
        )
        , menuInsert
    );
}

void
FOdysseyAnimationEditorTimelineTab::BuildImportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyAnimationEditorCommands::Get().ImportTextureSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.import-texture-sequence.name", "Texture Sequence...")
    );
    iMenuBuilder.AddMenuEntry(
        FOdysseyAnimationEditorCommands::Get().ImportImageSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.import-image-sequence.name", "Image Sequence...")
    );
}

void
FOdysseyAnimationEditorTimelineTab::BuildExportMenu(FMenuBuilder& iMenuBuilder)
{
    iMenuBuilder.AddMenuEntry(
        FOdysseyAnimationEditorCommands::Get().ExportImageSequence,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.export-image-sequence.name", "Image Sequence...")
    );
    iMenuBuilder.AddMenuEntry(
        FOdysseyAnimationEditorCommands::Get().ExportAsFlipbook,
        NAME_None,
        LOCTEXT("timeline-tab.file-menu.export-as-flipbook.name", "Flipbook...")
    );
}

void           
FOdysseyAnimationEditorTimelineTab::ImportTextureSequence()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "timeline-tab.import-texture-dialog.title", "Import Textures Sequence" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mExtension->Animation()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );
    assetsData.Sort();

    UOdysseyAnimation* animation = mExtension->Animation();

    if ( assetsData.Num() <= 0 )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));
    layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);
    
	layerImageRaster->Modify();
    
    FScopedSlowTask progressBar(assetsData.Num(), LOCTEXT("timeline-tab.import-texture-dialog.progress-bar.title", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    for( int i = 0; i < assetsData.Num(); i++ )
    {
        progressBar.EnterProgressFrame();
        UTexture2D* openedTexture = static_cast<UTexture2D*>(assetsData[i].GetAsset());
        TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(openedTexture, animation->Format()));    
	
		UOdysseyAnimationCellImageRaster* cell = Cast<UOdysseyAnimationCellImageRaster>(layerImageRaster->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
		TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
		FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
		::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
		rasterBlockMutator.EditTilesFromRects(
			{ invalidRect },
			[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
			{
				::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
				::ULIS::FEvent eventConvertFormat = FULISEventBuilder().RetainBlock(iBlock).RetainBlock(textureBlock).Build();
				ctx.ConvertFormat(*textureBlock, *iBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventConvertFormat);
				return { eventConvertFormat };
			}
		);
		rasterBlockMutator.Commit();
    }
    
    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(0);
    currentFrameMutator.Commit();
}

void           
FOdysseyAnimationEditorTimelineTab::ImportImageSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);

    UOdysseyAnimation* animation = animationSource->GetAnimation();
    UOdysseyAnimationLayerStack* layerStack = animation->GetLayerStack();
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool dialogValidated = desktopPlatformHandle->OpenFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("animation.import-image-sequence.dialog.title", "Select Images to import").ToString()
        , FPaths::ProjectDir()
        , animation->GetName()
        , TEXT("PNG Image (.png)|*.png|BMP Image (.bmp)|*.bmp|TGA Image (.tga)|*.tga|JPG Image (.jpg)|*.jpg")
        , EFileDialogFlags::Multiple
        , filenames
    );

    if (!dialogValidated || filenames.Num() <= 0)
        return;

    FScopedSlowTask progressBar(filenames.Num(), LOCTEXT("timeline-tab.import-image-sequence.progress-bar.title", "Importing Image Sequence"));
    progressBar.MakeDialog();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( animation->Format() );
    TArray<TSharedPtr<::ULIS::FBlock>> blocks;
    for (const FString& filename : filenames)
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

        if (block->Width() == animation->Width() && block->Height() == animation->Height() && block->Format() == animation->Format())
        {
            blocks.Add(block);
            continue;
        }
        
        //Need to convert the block before adding it to the layer
        TSharedPtr<::ULIS::FBlock> blockProxy = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), animation->Format());

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
		::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
		rasterBlockMutator.EditTilesFromRects(
			{ invalidRect },
			[&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
			{
				::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
				::ULIS::FEvent eventConvertFormat = FULISEventBuilder().RetainBlock(iBlock).RetainBlock(block).Build();
				ctx.ConvertFormat(*block, *iBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventConvertFormat);
				return { eventConvertFormat };
			}
		);
		rasterBlockMutator.Commit();
    }

    FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
    currentFrameMutator.Set(0);
    currentFrameMutator.Commit();
}

void           
FOdysseyAnimationEditorTimelineTab::ExportImageSequence()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);

    UOdysseyAnimation* animation = animationSource->GetAnimation();

    SOdysseyAnimationExportImageSequenceDialog::Open(animation);
}

void
FOdysseyAnimationEditorTimelineTab::ExportAsFlipbook()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
        return;

    TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);
    UOdysseyAnimation* animation = animationSource->GetAnimation();

    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "export-layers-as-textures.save-asset-dialog.title", "Export Layers As Texture" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(animation->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = animation->GetName() + TEXT("_Flipbook");
    saveAssetDialogConfig.AssetClassNames.Add( UPaperFlipbook::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if ( saveObjectPath == "" )
        return;

    FInt32Range frameRange = animation->GetFrameRange();
    int startFrame = frameRange.GetLowerBoundValue();
    int endFrame = frameRange.GetUpperBoundValue();

    FScopedSlowTask progressBar(endFrame - startFrame + 1, LOCTEXT("timeline-tab.export-as-flipbook.progress-bar.title", "Export As Flipbook"));
    progressBar.MakeDialog();

    // Create flipbook asset
    FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
    FString flipbookAssetName = FPaths::GetBaseFilename(saveObjectPath);
    FString flipbookPackagePath = assetPath + flipbookAssetName;
    UPackage* flipbookPackage = CreatePackage(*flipbookPackagePath);
    UPaperFlipbook* flipbook = NewObject<UPaperFlipbook>(flipbookPackage, UPaperFlipbook::StaticClass(), FName(*flipbookAssetName), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone | RF_Transactional);

    ETextureSourceFormat textureSourceFormat = TextureSourceFormatForULISFormat(animation->Format());    

    ::ULIS::eFormat blockFormat = ULISFormatForTextureSourceFormat(textureSourceFormat);
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = MakeShared<::ULIS::FBlock>(animation->Width(), animation->Height(), blockFormat);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(blockFormat);
    TArray<FGuid> lastRenderingComposition;

    FOdysseyFlipbookWrapper flipbookWrapper;
    flipbookWrapper.SetFlipbook(flipbook);

    int lastKeyFrameIndex = -1;
    int lastKeyFrameFirstFrame = -1;
    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();

        TArray<FGuid> renderingComposition = animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, i);
        if (renderingComposition == lastRenderingComposition)
            continue;

        lastRenderingComposition = renderingComposition;

        if (lastKeyFrameIndex >= 0)
            flipbookWrapper.SetKeyFrameLength(lastKeyFrameIndex, i - lastKeyFrameFirstFrame);
        
        //Render frame block
        TSharedPtr<IOdysseyImageRenderer> renderer = animation->BuildImageRenderer(IOdysseyImageRenderer::eRenderType::Render, i);
        renderer->Init();

        FOdysseyImageRendererCopyParams params(block, { block->Rect() });
        renderer->Copy(params, {});
        ctx.Finish();

        //FString assetName = FPaths::GetBaseFilename(saveObjectPath) + FString::Format(TEXT("_{0}"), { i });
        //UTexture2D* texture = CreateTextureFromBlock(block, textureSourceFormat, assetPath, FString assetName);

        int keyFrameIndex = flipbook->GetNumKeyFrames();

        FString textureName = flipbookAssetName + TEXT("_Texture_") + FString::Format(TEXT("{0}"), { i });
        FString spriteName = flipbookAssetName + TEXT("_Sprite_") + FString::Format(TEXT("{0}"), { i });

        FOdysseyTextureConfiguration textureConfiguration;
        textureConfiguration.Width = animation->Width();
        textureConfiguration.Height = animation->Height();
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

        //can be false on a FX Layer for example
        InitTextureWithBlockData(block.Get(), texture, textureSourceFormat);

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
	mutator.FramesPerSecond = animation->FramesPerSecond;

    FAssetRegistryModule::AssetCreated(flipbook);

    FSavePackageArgs packageArgs;
    packageArgs.SaveFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
    UPackage::SavePackage( flipbookPackage, flipbook, *flipbookAssetName, packageArgs );
        
    flipbookPackage->MarkAsFullyLoaded();
    flipbook->MarkPackageDirty();
}

void
FOdysseyAnimationEditorTimelineTab::CreateNewLayer()
{
    UOdysseyAnimation* animation = Animation();
    if (!animation)
        return;

    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    UOdysseyLayer* layer = nullptr;
    {
    #ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("timeline-tab.transaction.shortcut.create-new-layer", "Add Layer"));
    #endif
		layerStack->Modify();
        UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
        if (currentLayer)
        {
            if (currentLayer->CanHaveChildren && currentLayer->DisplayChildren)
            {
                layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), currentLayer);
            }
            else
            {
                UOdysseyLayer* parent = currentLayer->GetParent();
                int index = currentLayer->GetIndexInParent();
                layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass(), parent, index);
            }
        }
        else
        {
            layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
        }

        UOdysseyAnimationLayerImageRaster* animLayer = Cast<UOdysseyAnimationLayerImageRaster>(layer);
        if (!animLayer)
            return;

		animLayer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
		FOdysseyObjectEditorUtils::SetPropertyValue(animLayer, GET_MEMBER_NAME_CHECKED(UOdysseyAnimationLayer, CellsOffset), animation->CurrentFrame);
    
        FOdysseyAnimationCurrentFrameMutator currentFrameMutator(animation);
        currentFrameMutator.Set(animation->CurrentFrame);
        currentFrameMutator.Commit();
    }

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, GET_MEMBER_NAME_CHECKED( UOdysseyLayerStack, CurrentLayer), TSoftObjectPtr<UOdysseyLayer>(layer));
}

void
FOdysseyAnimationEditorTimelineTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( layerStack->CurrentLayer->GetIsLockedRecursively() )
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-tab.transaction.shortcut.set-layer-opacity", "Change Layer Opacity"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), GET_MEMBER_NAME_CHECKED( UOdysseyLayer, Opacity), FMath::Clamp(iOpacity, 0.f, 1.f));
}

#undef LOCTEXT_NAMESPACE
