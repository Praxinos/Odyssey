// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorLayerStackTab.h"

#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/SOdysseyAnimationPlaybackControls.h"
#include "Widgets/SOdysseyLayerStackAddLayerButton.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationCellsMutator.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "ULISLoaderModule.h"
#include "ULISEventBuilder.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorLayerStackTab"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorLayerStackTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorLayerStackTab::~FOdysseyAnimationEditorLayerStackTab()
{
}

FOdysseyAnimationEditorLayerStackTab::FOdysseyAnimationEditorLayerStackTab(FOdysseyAnimationEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyAnimationEditor_LayerStack")
    , LOCTEXT( "OdysseyAnimationEditorLayerStackTab", "Layer Stack" )
    , FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyAnimationEditorTab interface

TSharedPtr<SWidget>
FOdysseyAnimationEditorLayerStackTab::CreateWidget()
{
    return SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SOdysseyLayerStackAddLayerButton)
                    .LayerStack(LayerStack())
                    .OnAdded( this, &FOdysseyAnimationEditorLayerStackTab::OnLayerAdded)
                ]
                + SHorizontalBox::Slot()
                .FillWidth(1.f)
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    SNew(SOdysseyAnimationPlaybackControls, mEditor)
                    .PlaybackFramesPerSecond(this, &FOdysseyAnimationEditorLayerStackTab::PlaybackFramesPerSecond)
                ]
            ]
            + SVerticalBox::Slot()
            .FillHeight(1.0)
            [
                SNew(SOdysseyAnimationLayerStack, mEditor)
            ];
}

void
FOdysseyAnimationEditorLayerStackTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyAnimationEditorCommands& AnimationEditorCommands = FOdysseyAnimationEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyAnimationEditorLayerStackTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(AnimationEditorCommands.ImportTextureSequence, ImportTextureSequence )
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
FOdysseyAnimationEditorLayerStackTab::ExtendMenu(FToolMenuOwner iOwner, FName iMenuName)
{
    ExtendMenuFile(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditorLayerStackTab::LayerStack() const
{
    return mEditor->LayerStack();
}

UOdysseyAnimation*
FOdysseyAnimationEditorLayerStackTab::Animation() const
{
    return mEditor->Animation();
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditorLayerStackTab::Player() const
{
    return mEditor->Player();
}

float
FOdysseyAnimationEditorLayerStackTab::PlaybackFramesPerSecond() const
{
    return mEditor->PlaybackFramesPerSecond();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyAnimationEditorLayerStackTab::ExtendMenuFile( FToolMenuOwner iOwner, FName iMenuName )
{
    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".File")));

    FToolMenuSection& section = menu->AddSection("OdysseyAnimation", LOCTEXT("OdysseyAnimation", "Odyssey Animation"), FToolMenuInsert("FileLoadAndSave", EToolMenuInsertType::After));
    {
        section.AddMenuEntry( FOdysseyAnimationEditorCommands::Get().ImportTextureSequence );
    }
}

void           
FOdysseyAnimationEditorLayerStackTab::ImportTextureSequence()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures Sequence" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mEditor->Animation()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );
    assetsData.Sort();

    UOdysseyAnimation* animation = mEditor->Animation();

    if ( assetsData.Num() > 0 )
        layerStack->Modify();

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);
    if ( !layerImageRaster )
        return;
    
    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(animation->Format());
    for( int i = 0; i < assetsData.Num(); i++ )
    {
        UTexture2D* openedTexture = static_cast<UTexture2D*>(assetsData[i].GetAsset());
        TSharedPtr<::ULIS::FBlock> textureBlock = MakeShareable(NewBlockFromUTextureData(openedTexture, animation->Format()));
        if (textureBlock->Width() == animation->Width() && textureBlock->Height() == animation->Height() && textureBlock->Format() == animation->Format())
        {
            TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layerImageRaster, textureBlock);
            cells.Add(cell);
        }
        else
        {
            TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layerImageRaster, animation->Width(), animation->Height(), animation->Format());
            TSharedPtr<::ULIS::FBlock> cellBlock = cell->GetRasterBlock()->GetBlock();
            ::ULIS::FEvent eventConvertFormat = FULISEventBuilder().RetainBlock(cellBlock).RetainBlock(textureBlock).Build();
            ctx.ConvertFormat(*textureBlock, *cellBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventConvertFormat);
            ctx.Finish(); //avoids having too much blocks in memory at the same time
            cells.Add(cell);
        }
    }
    
    FOdysseyAnimationCellsMutator mutator(layerImageRaster);
    mutator.Add(cells);
    mutator.Commit();
}


void
FOdysseyAnimationEditorLayerStackTab::CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
}

void
FOdysseyAnimationEditorLayerStackTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( !FOdysseyObjectEditorUtils::HasProperty(layerStack->CurrentLayer.Get(), "Opacity") )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), "Opacity", FMath::Clamp(iOpacity, 0.f, 1.f));
}

void
FOdysseyAnimationEditorLayerStackTab::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, Animation()->Width(), Animation()->Height(), Animation()->Format());

#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Add Frame"));
#endif
        FOdysseyAnimationCellsMutator mutator(layer);
        mutator.Add({ cell });
        mutator.Commit();

        return;
    }
}

#undef LOCTEXT_NAMESPACE
