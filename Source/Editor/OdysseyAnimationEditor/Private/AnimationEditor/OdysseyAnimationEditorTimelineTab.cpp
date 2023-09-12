// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"

#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Cells/OdysseyAnimationCellsMutator.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "ULISEventBuilder.h"
#include "ULISLoaderModule.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "Misc/ScopedSlowTask.h"
#include "OdysseySurfaceTexture2DEditable.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorTimelineTab"

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
	: FOdysseyEditorTab(LOCTEXT( "OdysseyAnimationEditorTimelineTab", "Timeline" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Layers16" ))
    , mExtension(iExtension)
{
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
    return SNew(SOdysseyAnimationLayerStack, mExtension)
        .LayerStack(this, &FOdysseyAnimationEditorTimelineTab::LayerStack);
}

void
FOdysseyAnimationEditorTimelineTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyAnimationEditorCommands& AnimationEditorCommands = FOdysseyAnimationEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyAnimationEditorTimelineTab::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(AnimationEditorCommands.ImportTextureSequence, ImportTextureSequence )
    MAP_ACTION(AnimationEditorCommands.CreateNewAnimationLayerImageRaster, CreateNewLayer )
    MAP_ACTION(AnimationEditorCommands.StepForward, StepForward )
    MAP_ACTION(AnimationEditorCommands.StepBackward, StepBackward )
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

    FToolMenuSection& section = menu->AddSection("OdysseyAnimation", LOCTEXT("OdysseyAnimation", "Odyssey Animation"), FToolMenuInsert("FileLoadAndSave", EToolMenuInsertType::After));
    {
        section.AddMenuEntry( FOdysseyAnimationEditorCommands::Get().ImportTextureSequence );
    }
}

void           
FOdysseyAnimationEditorTimelineTab::ImportTextureSequence()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("LayerStack", "Import Textures Sequence"));

    FOpenAssetDialogConfig openAssetDialogConfig;
    openAssetDialogConfig.DialogTitleOverride = LOCTEXT( "ImportTextureDialogTitle", "Import Textures Sequence" );
    openAssetDialogConfig.DefaultPath = FPaths::GetPath(mExtension->Animation()->GetPathName() );
    openAssetDialogConfig.bAllowMultipleSelection = true;
    openAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    TArray < FAssetData > assetsData = contentBrowserModule.Get().CreateModalOpenAssetDialog( openAssetDialogConfig );
    assetsData.Sort();

    UOdysseyAnimation* animation = mExtension->Animation();

    if ( assetsData.Num() <= 0 )
        return;

    UOdysseyLayer* layer = layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
    UOdysseyAnimationLayerImageRaster* layerImageRaster = Cast<UOdysseyAnimationLayerImageRaster>(layer);
    if ( !layerImageRaster )
        return;
    
    layerStack->Modify();
    
    FScopedSlowTask progressBar(assetsData.Num(), LOCTEXT("LookingForUnusedAssetsText", "Importing Texture Sequence"));
    progressBar.MakeDialog();

    TArray<TSharedPtr<FOdysseyAnimationCell>> cells;
    for( int i = 0; i < assetsData.Num(); i++ )
    {
        progressBar.EnterProgressFrame();
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
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = cell->GetRasterBlock();
            FOdysseyRasterBlockMutator rasterBlockMutator(rasterBlock, false);
            ::ULIS::FRectI invalidRect = ::ULIS::FRectI::FromXYWH(0, 0, animation->Width(), animation->Height());
            rasterBlockMutator.EditTilesFromRects(
                { invalidRect },
                FOdysseyRasterBlockMutator::FEditDelegate::CreateLambda(
                    [&](TSharedPtr<::ULIS::FBlock> iBlock, const FULISInvalidTileMap& iTileMap) -> TArray<::ULIS::FEvent>
                    {
                        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
                        ::ULIS::FEvent eventConvertFormat = FULISEventBuilder().RetainBlock(iBlock).RetainBlock(textureBlock).Build();
                        ctx.ConvertFormat(*textureBlock, *iBlock, ::ULIS::FRectI::Auto, ::ULIS::FVec2I(0), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 0, nullptr, &eventConvertFormat);
                        return { eventConvertFormat };
                    }
                )
            );
            rasterBlockMutator.Commit();
            
            cells.Add(cell);
        }
    }
    
    FOdysseyAnimationCellsMutator mutator(layerImageRaster, layerImageRaster->GetCellsContainer());
    mutator.Add(cells);
    mutator.Commit();
}


void
FOdysseyAnimationEditorTimelineTab::CreateNewLayer()
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    layerStack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass());
}

void
FOdysseyAnimationEditorTimelineTab::ChangeLayerOpacity( float iOpacity )
{
    UOdysseyLayerStack* layerStack = LayerStack();
    if ( !layerStack )
        return;

    if ( !layerStack->CurrentLayer )
        return;

    if ( !FOdysseyObjectEditorUtils::HasProperty(layerStack->CurrentLayer.Get(), "Opacity") )
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(layerStack->CurrentLayer.Get(), "Opacity", FMath::Clamp(iOpacity, 0.f, 1.f));
}

void
FOdysseyAnimationEditorTimelineTab::StepForward()
{
    int frame = mExtension->Animation()->CurrentFrame + 1;
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), "CurrentFrame", frame);
}

void
FOdysseyAnimationEditorTimelineTab::StepBackward()
{
    int frame = FMath::Max(0, mExtension->Animation()->CurrentFrame - 1);
    FOdysseyObjectEditorUtils::SetPropertyValue(mExtension->Animation(), "CurrentFrame", frame);
}

void
FOdysseyAnimationEditorTimelineTab::OnLayerAdded(UOdysseyLayer* iLayer)
{
    if (iLayer->GetClass() == UOdysseyAnimationLayerImageRaster::StaticClass())
    {
        UOdysseyAnimationLayerImageRaster* layer = Cast<UOdysseyAnimationLayerImageRaster>(iLayer);
        TSharedPtr<FOdysseyAnimationCellImageRaster> cell = FOdysseyAnimationCellImageRaster::Create(layer, Animation()->Width(), Animation()->Height(), Animation()->Format());

#ifdef WITH_EDITOR
        FScopedTransaction ScopedTransaction(LOCTEXT("Layer Image Raster", "Add Frame"));
#endif
        FOdysseyAnimationCellsMutator mutator(layer, layer->GetCellsContainer());
        mutator.Add({ cell });
        mutator.Commit();

        return;
    }
}

#undef LOCTEXT_NAMESPACE
