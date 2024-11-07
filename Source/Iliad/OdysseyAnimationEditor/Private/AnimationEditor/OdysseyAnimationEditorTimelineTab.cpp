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

#include "Toolkits/BaseToolkit.h"
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
#include "OdysseyAnimationEditorFunctionLibrary.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    MapActions(toolkitCommands);
}

void
FOdysseyAnimationEditorTimelineTab::ExtendMenu(TSharedRef<FExtender> iExtender)
{
    ExtendMenuFile(iExtender);
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
FOdysseyAnimationEditorTimelineTab::MapActions( TSharedPtr<FUICommandList> iCommandList )
{
    const FOdysseyAnimationEditorCommands& AnimationEditorCommands = FOdysseyAnimationEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyAnimationEditorTimelineTab::__VA_ARGS__ ), FCanExecuteAction() );

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
FOdysseyAnimationEditorTimelineTab::ExtendMenuFile( TSharedRef<FExtender> iExtender )
{
    TSharedPtr<FUICommandList> commandList = MakeShared<FUICommandList>();
    MapActions(commandList);
    iExtender->AddMenuExtension(
        "OdysseyFile",
        EExtensionHook::After,
        commandList,
        FMenuExtensionDelegate::CreateLambda(
            [this](FMenuBuilder& iBuilder)
            {
                FOdysseyPainterEditor* editor = mExtension->GetEditor();
                if (!editor)
                    return;

                TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
                if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
                    return;

                iBuilder.BeginSection("OdysseyAnimation", LOCTEXT("timeline-tab.file-menu.animation-import-export-section.name", "Animation Import / Export"));
                {
                    iBuilder.AddSubMenu(
                        LOCTEXT("timeline-tab.file-menu.import-submenu.name", "Import"),
                        LOCTEXT("timeline-tab.file-menu.import-submenu.tooltip", "Contains Import actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyAnimationEditorTimelineTab::BuildImportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Import" )
                    );

                    iBuilder.AddSubMenu(
                        LOCTEXT("timeline-tab.file-menu.export-submenu.name", "Export"),
                        LOCTEXT("timeline-tab.file-menu.export-submenu.tooltip", "Contains Export actions"),
                        FNewMenuDelegate::CreateRaw(this, &FOdysseyAnimationEditorTimelineTab::BuildExportMenu),
                        false,
                        FSlateIcon( "OdysseyStyle", "AnimationEditor.File-Menu.Export" )
                    );
                }
                iBuilder.EndSection();
            }
        )
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

    TArray<UTexture2D*> textures;
    for(FAssetData& assetData : assetsData)
    {
        textures.Add(Cast<UTexture2D>(assetsData[0].GetAsset()));
    }

    UOdysseyAnimationEditorAnimationFunctionLibrary::ImportTextureSequence(animation, textures);
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

    filenames.Sort(
       [](const FString& iA, const FString& iB)
       {
           return iA < iB;
       }
    );

    UOdysseyAnimationEditorAnimationFunctionLibrary::ImportImageSequence(animation, filenames);
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

    FString assetPath = FPaths::GetPath(saveObjectPath) + "/";
    FString flipbookAssetName = FPaths::GetBaseFilename(saveObjectPath);

    UOdysseyAnimationEditorAnimationFunctionLibrary::ExportAsFlipbook(animation, animation->GetFrameRange(), flipbookAssetName, assetPath);
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

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack, GET_MEMBER_NAME_CHECKED( UOdysseyLayerStack, CurrentLayer), layer);
}

void
FOdysseyAnimationEditorTimelineTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( layerStack->CurrentLayer->IsLockedRecursively() )
        return;

#ifdef WITH_EDITOR
    FScopedTransaction ScopedTransaction(LOCTEXT("timeline-tab.transaction.shortcut.set-layer-opacity", "Change Layer Opacity"));
#endif
    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), GET_MEMBER_NAME_CHECKED( UOdysseyLayer, Opacity), FMath::Clamp(iOpacity, 0.f, 1.f));
}

#undef LOCTEXT_NAMESPACE
