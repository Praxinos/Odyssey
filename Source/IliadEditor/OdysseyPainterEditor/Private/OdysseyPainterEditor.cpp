// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditor.h"

#include "FileHelpers.h"
#include "Framework/Commands/GenericCommands.h"
#include "Toolkits/AssetEditorModeUILayer.h"
#include "Toolkits/BaseToolkit.h"
#include "ToolMenu.h"
#include "ToolMenus.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"

#include "HUD/OdysseyVectorHUD.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyAnimationCurrentFrameMutator.h"
#include "OdysseyAnimationGlobalShortcuts.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyBlockClipboardData.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyCoreEditorModule.h"
#include "OdysseyEditorLayoutBuilder.h"
#include "OdysseyHUDElement.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerCell.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLayerStackGlobalShortcuts.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditorAnimationFlipSystem.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyPainterEditorBrushContext.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorExtension.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "OdysseyPainterEditorFlipbookListener.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorGlobalShortcuts.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditorToolConfiguration.h"
#include "OdysseyPainterEditorToolMenuContext.h"
#include "OdysseyPalette.h"
#include "OdysseyPaletteEntryColor.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStackUserData.h"
#include "OdysseyToolCollection.h"
#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorTagInbetweener.h"
#include "Palette/OdysseyPalette.h"
#include "Proxies/OdysseyBrushColor.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "ULISLoaderModule.h"
#include "Undo/OdysseyVectorUndoGroup.h"
#include "Undo/OdysseyVectorUndoUngroup.h"
#include "Undo/OdysseyVectorUndoObjectTransform.h"
#include "Undo/OdysseyVectorUndoRemoveObjects.h"
#include "Undo/OdysseyVectorUndoBucketRemove.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "Undo/OdysseyVectorUndoPathStitch.h"
#include "Undo/OdysseyVectorUndoObjectAdd.h"
#include "Undo/OdysseyVectorUndoApplyTransformations.h"
#include "Undo/OdysseyVectorUndoVertexAlignment.h"
#include "Undo/OdysseyVectorUndoVertexLock.h"
#include "Undo/OdysseyVectorUndoSelectObject.h"
#include "Undo/OdysseyVectorUndoSelectVertex.h"
#include "Undo/OdysseyVectorUndoTransferObjects.h"
#include "Undo/OdysseyVectorUndoPathAlter.h"
#include "Undo/OdysseyVectorUndoTagAdd.h"
#include "Undo/OdysseyVectorUndoTagRemove.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerChartAlter.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerCommit.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerMatching.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerReset.h"

//Tools
#include "OdysseyPainterEditorAnimationOutOfPegsTool.h"
#include "OdysseyPainterEditorColorPickerTool.h"
#include "OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyPainterEditorRasterEraserTool.h"
#include "OdysseyPainterEditorRasterLiquifyTool.h"
#include "OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "OdysseyPainterEditorRasterSelectionTool.h"
#include "OdysseyPainterEditorRasterTransformTool.h"
#include "OdysseyPainterEditorRasterPaintBucketTool.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorVectorChartTool.h"
#include "OdysseyPainterEditorVectorCutTool.h"
#include "OdysseyPainterEditorVectorEraserTool.h"
#include "OdysseyPainterEditorVectorGridTool.h"
#include "OdysseyPainterEditorVectorMatchingTool.h"
#include "OdysseyPainterEditorVectorPaintBucketTool.h"
#include "OdysseyPainterEditorVectorPathDrawingTool.h"
#include "OdysseyPainterEditorVectorPathEditTool.h"
#include "OdysseyPainterEditorVectorPathPushTool.h"
#include "OdysseyPainterEditorVectorPathSmoothTool.h"
#include "OdysseyPainterEditorVectorPathStitchTool.h"
#include "OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "OdysseyPainterEditorVectorScenePanTool.h"
#include "OdysseyPainterEditorVectorSelectionTool.h"
#include "OdysseyPainterEditorVectorTrajectoryTool.h"
#include "OdysseyPainterEditorVectorTransformTool.h"

// Tabs Includes
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorAnimationLighttableTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolCollectionTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"

//Tablet Includes
#include "SOdysseyTabletAPISwitcher.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

TSharedPtr<::ULIS::FBlock> FOdysseyPainterEditor::mCopyBlock = nullptr;

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{
    OnClose();
}

FOdysseyPainterEditor::FOdysseyPainterEditor(TSharedRef<FBaseToolkit> iToolkit)
    : mToolkit(iToolkit)
    , mName("OdysseyPainterEditor")
    , mSource(nullptr)
    , mMeshSelector(MakeShared<FOdysseyMeshSelector>())
    , mCurrentMainTool(nullptr)
    , mCurrentTemporaryTool(nullptr)
    , mVectorHUDFlags(FOdysseyVectorHUD::HUD_MODE_OBJECT)
    , mVectorDrawingFlags(0)
    , mHUDSystem(MakeShared<FOdysseyHUDElement>())
    , mToolsHUD(MakeShared<FOdysseyHUDElement>())
    , mRasterSelectionHUD(MakeShared<FOdysseyHUDElement>())
    , mRasterSelection(NewObject< UOdysseyPainterEditorRasterSelection >())
    , mBrushContexts()
    , mPaintColor(::ULIS::FColor::Black)
    , mAnimationTimelinePosition(MakeShared<FOdysseyPainterEditorAnimationTimelinePosition>())
    , mRasterDrawingTool(nullptr)
    , mRasterEraserTool(nullptr)
    , mRasterSelectionTool(nullptr)
    , mRasterTransformTool(nullptr)
    , mRasterPrimitiveDrawingTool(nullptr)
    , mRasterLiquifyTool(nullptr)
    , mVectorPrimitiveDrawingTool(nullptr)
    , mVectorPathDrawingTool(nullptr)
    , mVectorPathEditTool(nullptr)
    , mVectorSelectionTool(nullptr)
    , mVectorCutTool(nullptr)
    , mVectorScenePanTool(nullptr)
    , mVectorEraserTool(nullptr)
    , mVectorPathPushTool(nullptr)
    , mVectorPathSmoothTool(nullptr)
    , mVectorPathStitchTool(nullptr)
    , mVectorPaintBucketTool(nullptr)
    , mColorPickerTool(nullptr)
    , mVectorGridTool(nullptr)
    , mVectorTransformTool(nullptr)
    , mVectorMatchingTool(nullptr)
    , mVectorChartTool(nullptr)
    , mOutOfPegsTool(nullptr)
    , mTemporaryColorPickerTool( nullptr )
    , mRecentTools( NewObject<UOdysseyToolCollection>(GetTransientPackage(), NAME_None, RF_Transient) )
    , mAnimationFlipSystem(MakeShared<FOdysseyPainterEditorAnimationFlipSystem>(this))
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyPainterEditor::OnCurrentLayerChanged);
    mBrushContexts.Add(new FOdysseyPainterEditorBrushContext(this));
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyPainterEditor::Initialize()
{
    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    painterEditorModule.AddOpenedEditor(this);

    InitHUD();
    InitTools();
    InitTabs();
    InitShortcuts();

    //Init the shortcuts
    FSlateApplication::Get().RegisterInputPreProcessor(mAnimationFlipSystem);

    //Init the extensions
    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Initialize();
}

void
FOdysseyPainterEditor::InitHUD()
{
    mToolsHUD->SetIsVisible(MakeAttributeSP(this, &FOdysseyPainterEditor::IsToolsHUDVisible));
    mRasterSelectionHUD->SetIsVisible(MakeAttributeSP(this, &FOdysseyPainterEditor::IsRasterSelectionHUDVisible));
    mHUDSystem->AddElement(mRasterSelectionHUD);
    mHUDSystem->AddElement(mToolsHUD);
}

void
FOdysseyPainterEditor::InitTools()
{
    mRasterDrawingTool = AddMainTool<UOdysseyPainterEditorRasterDrawingTool>();
    mRasterEraserTool = AddMainTool<UOdysseyPainterEditorRasterEraserTool>();
    mRasterSelectionTool = AddMainTool<UOdysseyPainterEditorRasterSelectionTool>();
    mRasterTransformTool = AddMainTool<UOdysseyPainterEditorRasterTransformTool>();
    mRasterPaintBucketTool = AddMainTool<UOdysseyPainterEditorRasterPaintBucketTool>();
    mRasterPrimitiveDrawingTool = AddMainTool<UOdysseyPainterEditorRasterPrimitiveDrawingTool>();
    mRasterLiquifyTool = AddMainTool<UOdysseyPainterEditorRasterLiquifyTool>();
    mVectorPathDrawingTool = AddMainTool<UOdysseyPainterEditorVectorPathDrawingTool>();
    mVectorPathEditTool = AddMainTool<UOdysseyPainterEditorVectorPathEditTool>();
    mVectorPrimitiveDrawingTool = AddMainTool<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
    mVectorSelectionTool = AddMainTool<UOdysseyPainterEditorVectorSelectionTool>();
    mVectorCutTool = AddMainTool<UOdysseyPainterEditorVectorCutTool>();
    mVectorScenePanTool = AddMainTool<UOdysseyPainterEditorVectorScenePanTool>();
    mVectorEraserTool = AddMainTool<UOdysseyPainterEditorVectorEraserTool>();
    mVectorPathPushTool = AddMainTool<UOdysseyPainterEditorVectorPathPushTool>();
    mVectorPathSmoothTool = AddMainTool<UOdysseyPainterEditorVectorPathSmoothTool>();
    mVectorPathStitchTool = AddMainTool<UOdysseyPainterEditorVectorPathStitchTool>();
    mVectorPaintBucketTool = AddMainTool<UOdysseyPainterEditorVectorPaintBucketTool>();
    mColorPickerTool = AddMainTool<UOdysseyPainterEditorColorPickerTool>();
    mVectorGridTool = AddMainTool<UOdysseyPainterEditorVectorGridTool>();
    mVectorTransformTool = AddMainTool<UOdysseyPainterEditorVectorTransformTool>();
    mVectorMatchingTool = AddMainTool<UOdysseyPainterEditorVectorMatchingTool>();
    mVectorChartTool = AddMainTool<UOdysseyPainterEditorVectorChartTool>();
    mVectorTrajectoryTool = AddMainTool<UOdysseyPainterEditorVectorTrajectoryTool>();

    mOutOfPegsTool = AddTemporaryTool<UOdysseyPainterEditorAnimationOutOfPegsTool>();
    mTemporaryColorPickerTool = AddTemporaryTool<UOdysseyPainterEditorColorPickerTool>();
    mTemporaryColorPickerTool->mIsTemporaryTool = true;

    mRasterDrawingTool->SetBrushContexts(&mBrushContexts);
}

void
FOdysseyPainterEditor::InitTabs()
{
    TSharedRef<FOdysseyPainterEditorMeshSelectorTab> meshSelectorTab = MakeShared<FOdysseyPainterEditorMeshSelectorTab>(this);
    TSharedRef<FOdysseyPainterEditorViewportTab> viewportTab = MakeShared<FOdysseyPainterEditorViewportTab>(this);
    TSharedRef<FOdysseyPainterEditorColorSelectorTab> colorSelectorTab = MakeShared<FOdysseyPainterEditorColorSelectorTab>(this);
    TSharedRef<FOdysseyPainterEditorToolsTab> toolsTab = MakeShared<FOdysseyPainterEditorToolsTab>(this);
    TSharedRef<FOdysseyPainterEditorToolCollectionTab> toolCollectionTab = MakeShared<FOdysseyPainterEditorToolCollectionTab>(this);
    TSharedRef<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = MakeShared<FOdysseyPainterEditorVectorSceneTreeViewTab>(this);
    TSharedRef<FOdysseyPainterEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyPainterEditorLayerStackTab>(this);
    TSharedRef<FOdysseyPainterEditorTextureDetailsTab> textureDetailsTab = MakeShared<FOdysseyPainterEditorTextureDetailsTab>(this);
    TSharedRef<FOdysseyPainterEditorFlipbookTimelineTab> flipbookTimelineTab = MakeShared<FOdysseyPainterEditorFlipbookTimelineTab>(this);
    TSharedRef<FOdysseyPainterEditorAnimationTimelineTab> animationTimelineTab = MakeShared<FOdysseyPainterEditorAnimationTimelineTab>(this);
    TSharedRef<FOdysseyPainterEditorAnimationLighttableTab> animationLighttableTab = MakeShared<FOdysseyPainterEditorAnimationLighttableTab>(this);
    TSharedRef<FOdysseyPainterEditorAnimationDetailsTab> animationDetailsTab = MakeShared<FOdysseyPainterEditorAnimationDetailsTab>(this);

    AddTab(toolsTab);
    AddTab(toolCollectionTab);
    AddTab(meshSelectorTab);
    AddTab(viewportTab);
    AddTab(colorSelectorTab);
    AddTab(vectorSceneTreeViewTab);
    AddTab(layerStackTab);
    AddTab(textureDetailsTab);
    AddTab(flipbookTimelineTab);
    AddTab(animationTimelineTab);
    AddTab(animationLighttableTab);
    AddTab(animationDetailsTab);

    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->Init();
    }
}

void
FOdysseyPainterEditor::InitShortcuts()
{
    TAttribute<UOdysseyAnimation*> animation = MakeAttributeRaw(this, &FOdysseyPainterEditor::GetAnimation);
    TAttribute<UOdysseyLayerStack*> layerStack = MakeAttributeRaw(this, &FOdysseyPainterEditor::LayerStack);
    TAttribute<int> currentFrame = MakeAttributeLambda(
        [this]() -> int
        {
            UOdysseyAnimationPlayer* player =  GetAnimationPlayer();
            if (!player)
                return 0;

            return player->GetCurrentFrame().FrameNumber.Value;
        }
    );
    auto onTransactCurrentFrame = FOdysseyAnimationGlobalShortcuts::FOnTransactCurrentFrame::CreateLambda(
        [this, currentFrame](TOptional<int> iFrame)
        {
            UOdysseyAnimationPlayer* player =  GetAnimationPlayer();
            if (!player)
                return;

            int frame = iFrame.Get(currentFrame.Get());

            FOdysseyAnimationCurrentFrameMutator currentFrameMutator(player);
            currentFrameMutator.Set(frame);
            currentFrameMutator.Commit();
        }
    );

    GetShortcuts().Add(MakeShared<FOdysseyPainterEditorGlobalShortcuts>(this));
    GetShortcuts().Add(MakeShared<FOdysseyLayerStackGlobalShortcuts>(layerStack));
    GetShortcuts().Add(MakeShared<FOdysseyAnimationGlobalShortcuts>(animation, currentFrame, onTransactCurrentFrame));
}

FOdysseyPainterEditor::FOnAddEditedObject&
FOdysseyPainterEditor::OnAddEditedObjectDelegate()
{
    return mOnAddEditedObject;
}

void
FOdysseyPainterEditor::SetEditedObject(UObject* iObject)
{
    if (mEditedObject && mEditedObject->IsA<UPaperFlipbook>())
    {
        UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(mEditedObject);
        mFlipbookListener = nullptr;
        for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
        {
            UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(flipbook, index);
            if (!sprite)
                continue;

            RemoveEditedObject(sprite);

            UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, index);
            if (!texture)
                continue;

            RemoveEditedObject(texture);
        }
    }

    mEditedObject = iObject;
    if (!iObject)
    {
        SetSource(nullptr);
        return;
    }

    if (mEditedObject->IsA<UOdysseyAnimation>())
    {
        TSharedPtr<FOdysseyPainterEditorAnimationSource> source = MakeShared<FOdysseyPainterEditorAnimationSource>(Cast<UOdysseyAnimation>(mEditedObject));
        SetSource(source);
    }
    else if (mEditedObject->IsA<UTexture2D>())
    {
        TSharedPtr<FOdysseyPainterEditorTextureSource> source = MakeShared<FOdysseyPainterEditorTextureSource>(Cast<UTexture2D>(mEditedObject));
        SetSource(source);
    }
    else if (mEditedObject->IsA<UPaperFlipbook>())
    {
        UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(mEditedObject);
        mFlipbookListener = MakeShared<FOdysseyPainterEditorFlipbookListener>(flipbook);
        mFlipbookListener->OnSpriteTextureChanged().AddRaw(this, &FOdysseyPainterEditor::OnFlipbookSpriteTextureChanged);

        //Find all additional Edited Objects (Sprites and Textures)
        for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
        {
            UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(flipbook, index);
            if (!sprite)
                continue;

            AddEditedObject(sprite);

            UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, index);
            if (!texture)
                continue;

            AddEditedObject(texture);
        }

        //Set Texture Source if needed
        if (flipbook->GetNumKeyFrames() > 0)
        {
            UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, 0);
            TSharedPtr<FOdysseyPainterEditorTextureSource> source = MakeShared<FOdysseyPainterEditorTextureSource>(texture);
            SetSource(source);
        }
    }
}

void
FOdysseyPainterEditor::OnFlipbookSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    if (!mEditedObject->IsA<UPaperFlipbook>())
        return;

    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(mEditedObject);

    UTexture2D* texture = iSprite->GetSourceTexture();
    for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
    {
        UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(flipbook, i);
        if (sprite == iSprite)
        {
            if (iOldTexture)
                RemoveEditedObject(iOldTexture);

            if (texture)
                AddEditedObject(texture);
        }
    }

    TSharedPtr<FOdysseyPainterEditorFlipbookTimelineTab> timelineTab = FindTab<FOdysseyPainterEditorFlipbookTimelineTab>();
    int32 index = timelineTab->Timeline()->GetCurrentKeyframeIndex();

    UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(flipbook, index);
    if (sprite != iSprite)
        return;

    TSharedPtr<FOdysseyPainterEditorTextureSource> source = MakeShared<FOdysseyPainterEditorTextureSource>(texture);
    SetSource(source);
}


FOdysseyPainterEditor::FOnRemoveEditedObject&
FOdysseyPainterEditor::OnRemoveEditedObjectDelegate()
{
    return mOnRemoveEditedObject;
}

FSimpleDelegate&
FOdysseyPainterEditor::OnRegenerateToolbarAndMenus()
{
    return mOnRegenerateToolbarAndMenus;
}

TArray<UObject*>
FOdysseyPainterEditor::GetAdditionalEditedObjects()
{
    return mAdditionalEditedObjects;
}

FOdysseyEditorShortcuts&
FOdysseyPainterEditor::GetShortcuts()
{
    return mShortcuts;
}

void
FOdysseyPainterEditor::AddTab(TSharedRef<FOdysseyEditorTab> iTab)
{
    mTabs.Add(iTab);
}

const TArray<TSharedPtr<FOdysseyEditorTab>>&
FOdysseyPainterEditor::GetTabs() const
{
    return mTabs;
}

void
FOdysseyPainterEditor::CloseAllTabs()
{
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        if (tab->IsOpened())
            tab->Close();
    }
}

void
FOdysseyPainterEditor::RegisterTabSpawners( const TSharedRef< FTabManager >& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(FText::FromName(mName));
    TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->SetTabManager(iTabManager);
        tab->Register(workspaceMenuCategoryRef);
    }
}

void
FOdysseyPainterEditor::UnregisterTabSpawners( const TSharedRef< FTabManager >& iTabManager )
{
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->Unregister();
    }
}

const FName&
FOdysseyPainterEditor::GetId() const
{
    return mName;
}

UObject*
FOdysseyPainterEditor::GetEditedObject() const
{
    return mEditedObject;
}

void
FOdysseyPainterEditor::AddEditedObject(UObject* iObject)
{
    mAdditionalEditedObjects.Add(iObject);
    mOnAddEditedObject.Broadcast(iObject);
}

void
FOdysseyPainterEditor::RemoveEditedObject(UObject* iObject)
{
    mAdditionalEditedObjects.Remove(iObject);
    mOnRemoveEditedObject.Broadcast(iObject);
}

void
FOdysseyPainterEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    mShortcuts.MapActionsToCommandList(toolkitCommands);

    //TODO: Use only mShortcuts instead of BindShortcuts (better coding style)
    for (TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->BindShortcuts(iToolkit);
    }

    //---

    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.SwitchTabletAPI, SwitchTabletAPI )

    //Need to rethink the commands and shortcuts to put them in the right place and not in GUI
    MAP_ACTION(painterEditorCommands.ClearCurrentLayer, ClearCurrentLayerOrSelection)

    MAP_ACTION(painterEditorCommands.ToggleEraserButton, ToggleEraserButton)

    #undef MAP_ACTION

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->BindShortcuts(iToolkit);
}


void
FOdysseyPainterEditor::SwitchTabletAPI()
{
    SOdysseyTabletAPISwitcher::Open();
}

void
FOdysseyPainterEditor::ClearCurrentLayerOrSelection()
{
    if( !mRasterSelection->IsEmpty() && GetCurrentTool()->IsA(UOdysseyPainterEditorRasterBaseTool::StaticClass()))
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();
        if (mediaRasters.Num() <= 0)
            return;

        FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

        TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> block = rasterBlock->GetBlock();

        ::ULIS::FRectI boundingBox = rasterBlock->GetRect();

        TSharedPtr<::ULIS::FBlock> copyBlock = MakeShared<::ULIS::FBlock>(boundingBox.w, boundingBox.h, rasterBlock->GetFormat());

        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(rasterBlock->GetFormat());
        ::ULIS::FEvent clearEvent, copyEvent;
        ctx.Clear(*copyBlock);
        ctx.Finish();

        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> maskBlock = mRasterSelection->GetBlock();

        if (maskBlock)
        {
            ctx.Copy(
                *block,
                *copyBlock,
                boundingBox,
                ::ULIS::FVec2I(0, 0),
                ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                0,
                nullptr,
                &copyEvent
            );

            ctx.FilterInto(
                [](const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels)
                {
                    for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
                    {
                        iDstPixel.SetAlphaF(iDstPixel.AlphaF() * iSrcPixel.GreyF());
                    }
                }
                , *maskBlock
                , *copyBlock
                , boundingBox
                , ::ULIS::FVec2I(0, 0)
                , ::ULIS::FSchedulePolicy::MultiScanlines
                , 1
                , &copyEvent
                , nullptr
            );

            ctx.Finish();
        }

        mSource->ClearFromCopyBlock(copyBlock);

        return;
    }

    if( mSource )
        mSource->Clear();
}

void
FOdysseyPainterEditor::ToggleEraserButton()
{
    if( GetCurrentTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()) )
    {
        UOdysseyPainterEditorRasterDrawingTool* currentTool = Cast< UOdysseyPainterEditorRasterDrawingTool>(GetCurrentTool());
        currentTool->BlendParameters.bEraserMode = !currentTool->BlendParameters.bEraserMode;
    }
}

void
FOdysseyPainterEditor::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->ExtendMenu( iExtender );
    }

    for (TObjectPtr<UOdysseyPainterEditorTool> tool : mMainTools)
    {
        tool->ExtendMenu(iExtender);
    }

    for (TObjectPtr<UOdysseyPainterEditorTool> tool : mTemporaryTools)
    {
        tool->ExtendMenu(iExtender);
    }

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->ExtendMenu(iExtender);
}

void
FOdysseyPainterEditor::InitToolMenuContext(FToolMenuContext& MenuContext)
{
    UOdysseyPainterEditorToolMenuContext* Context = NewObject<UOdysseyPainterEditorToolMenuContext>();
    Context->PainterEditor = this;
    MenuContext.AddObject(Context);
}

void
FOdysseyPainterEditor::ExtendLevelEditorToolbar(UToolMenu* iToolbar)
{
    mToolbarMenuName = iToolbar->GetMenuName();

    iToolbar->AddDynamicSection(
        "ToolParameters",
        FNewToolMenuDelegate::CreateLambda(
            [this](UToolMenu* iToolMenu)
            {
                ExtendToolbarSaveAssetButton(iToolMenu);
                ExtendToolbarToolParameters(iToolMenu);
            }
        )
    );
}

void
FOdysseyPainterEditor::ExtendAssetEditorToolbar(UToolMenu* iToolbar)
{
    mToolbarMenuName = iToolbar->GetMenuName();

    iToolbar->AddDynamicSection(
        "ToolParameters",
        FNewToolMenuDelegate::CreateLambda(
            [](UToolMenu* iToolMenu)
            {
                const UOdysseyPainterEditorToolMenuContext* Context = iToolMenu->FindContext<UOdysseyPainterEditorToolMenuContext>();
                if (!Context)
                    return;

                FOdysseyPainterEditor* painterEditor = Context->PainterEditor;
                if (!painterEditor)
                    return;

                painterEditor->ExtendToolbarToolParameters(iToolMenu);
            }
        )
    );
}

void
FOdysseyPainterEditor::ExtendToolbarSaveAssetButton(UToolMenu* iToolMenu)
{
    FToolMenuSection& assetSection = iToolMenu->AddSection("Asset");

    assetSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            NAME_None,
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        TArray<UPackage*> packages;
                        UObject* editedObject = GetEditedObject();
                        if (editedObject)
                            packages.Add(editedObject->GetOutermost());

                        TArray<UObject*> additionalEditedObjects = GetAdditionalEditedObjects();
                        for( UObject* additionalEditedObject : additionalEditedObjects )
                        {
                            packages.Add( additionalEditedObject->GetOutermost() );
                        }

                        FEditorFileUtils::PromptForCheckoutAndSave(packages, true, false);
                    }
                )
            ),
            FText(),
            LOCTEXT("top-tab.save-asset", "Saves the painted asset"),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Save32"),
            EUserInterfaceActionType::Button
        )
    );
}

void
FOdysseyPainterEditor::ExtendToolbarToolParameters(UToolMenu* iToolMenu)
{
    FToolMenuSection& undoRedoSection = iToolMenu->AddSection("UndoRedo");

    undoRedoSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "Undo",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        if( GetCurrentMainTool() && GetCurrentMainTool()->IsA( UOdysseyPainterEditorRasterTransformTool::StaticClass() ) )
                            Cast<UOdysseyPainterEditorRasterTransformTool>( GetCurrentMainTool() )->UndoTransformTransaction();
                        else
                            GEditor->UndoTransaction(true);
                    }
                )
            ),
            FText(),
            LOCTEXT("top-tab.undo", "Undo the previous action."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Undo32"),
            EUserInterfaceActionType::Button
        )
    );

    undoRedoSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "Redo",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        if( GetCurrentMainTool() && GetCurrentMainTool()->IsA( UOdysseyPainterEditorRasterTransformTool::StaticClass() ) )
                            Cast<UOdysseyPainterEditorRasterTransformTool>( GetCurrentMainTool() )->RedoTransformTransaction();
                        else
                            GEditor->RedoTransaction();
                    }
                )
            ),
            FText(),
            LOCTEXT("top-tab.redo", "Redo the next action."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Redo32"),
            EUserInterfaceActionType::Button
        )
    );

    FToolMenuSection& clearCanvasSection = iToolMenu->AddSection("ClearCanvas");

    clearCanvasSection.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            "ClearCanvas",
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        ClearCurrentLayerOrSelection();
                    }
                )
            ),
            FText(),
            LOCTEXT("top-tab.clear", "Clear the whole canvas."),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Clear32"),
            EUserInterfaceActionType::Button
        )
    );

    UOdysseyPainterEditorTool* currentTool = GetCurrentTool();
    if (currentTool)
    {
        currentTool->ExtendToolbar(iToolMenu);
    }
}

void
FOdysseyPainterEditor::OnClose()
{
    for(TSharedPtr<FOdysseyEditorTab> tab : mTabs )
        tab->CloseTab();

    //Here is where we should clean everything prior to editor destruction
    mTabs.Empty(); //ensure all tabs are destroyed, because some need the editor on destruction

    SetSource(nullptr);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Finalize();

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    FSlateApplication::Get().UnregisterInputPreProcessor(mAnimationFlipSystem);

    mHUDSystem = nullptr;
    mToolsHUD = nullptr;
    mRasterSelectionHUD = nullptr;
    mRecentTools = nullptr;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    painterEditorModule.RemoveOpenedEditor(this);
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentMainToolChanged()
{
    return mOnCurrentMainToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentTemporaryToolChanged()
{
    return mOnCurrentTemporaryToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnCurrentToolChanged()
{
    return mOnCurrentToolChanged;
}

FSimpleMulticastDelegate&
FOdysseyPainterEditor::OnSourceChanged()
{
    return mOnSourceChanged;
}

TSharedPtr<FOdysseyPainterEditorSource>
FOdysseyPainterEditor::GetSource() const
{
    return mSource;
}

TSharedPtr<FBaseToolkit>
FOdysseyPainterEditor::GetToolkit() const
{
    return mToolkit.Pin();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyPainterEditorRasterDrawingTool*
FOdysseyPainterEditor::GetRasterDrawingTool() const
{
    return mRasterDrawingTool;
}

UOdysseyPainterEditorRasterEraserTool*
FOdysseyPainterEditor::GetRasterEraserTool() const
{
    return mRasterEraserTool;
}

UOdysseyPainterEditorRasterSelectionTool*
FOdysseyPainterEditor::GetRasterSelectionTool() const
{
    return mRasterSelectionTool;
}

UOdysseyPainterEditorRasterTransformTool*
FOdysseyPainterEditor::GetRasterTransformTool() const
{
    return mRasterTransformTool;
}

UOdysseyPainterEditorRasterPrimitiveDrawingTool*
FOdysseyPainterEditor::GetRasterPrimitiveDrawingTool() const
{
    return mRasterPrimitiveDrawingTool;
}

UOdysseyPainterEditorRasterLiquifyTool*
FOdysseyPainterEditor::GetRasterLiquifyTool() const
{
    return mRasterLiquifyTool;
}

UOdysseyPainterEditorRasterPaintBucketTool*
FOdysseyPainterEditor::GetRasterPaintBucketTool() const
{
    return mRasterPaintBucketTool;
}

UOdysseyPainterEditorVectorPrimitiveDrawingTool*
FOdysseyPainterEditor::GetVectorPrimitiveDrawingTool() const
{
    return mVectorPrimitiveDrawingTool;
}

UOdysseyPainterEditorVectorPathDrawingTool*
FOdysseyPainterEditor::GetVectorPathDrawingTool() const
{
    return mVectorPathDrawingTool;
}

UOdysseyPainterEditorVectorPathEditTool*
FOdysseyPainterEditor::GetVectorPathEditTool() const
{
    return mVectorPathEditTool;
}

UOdysseyPainterEditorVectorSelectionTool*
FOdysseyPainterEditor::GetVectorSelectionTool() const
{
    return mVectorSelectionTool;
}

UOdysseyPainterEditorVectorCutTool*
FOdysseyPainterEditor::GetVectorCutTool() const
{
    return mVectorCutTool;
}

UOdysseyPainterEditorVectorGridTool*
FOdysseyPainterEditor::GetVectorGridTool() const
{
    return mVectorGridTool;
}

UOdysseyPainterEditorVectorTransformTool*
FOdysseyPainterEditor::GetVectorTransformTool() const
{
    return mVectorTransformTool;
}

UOdysseyPainterEditorVectorMatchingTool*
FOdysseyPainterEditor::GetVectorMatchingTool() const
{
    return mVectorMatchingTool;
}

UOdysseyPainterEditorVectorTrajectoryTool*
FOdysseyPainterEditor::GetVectorTrajectoryTool() const
{
    return mVectorTrajectoryTool;
}

UOdysseyPainterEditorVectorChartTool*
FOdysseyPainterEditor::GetVectorChartTool() const
{
    return mVectorChartTool;
}

UOdysseyPainterEditorVectorScenePanTool*
FOdysseyPainterEditor::GetVectorScenePanTool() const
{
    return mVectorScenePanTool;
}

UOdysseyPainterEditorVectorEraserTool*
FOdysseyPainterEditor::GetVectorEraserTool() const
{
    return mVectorEraserTool;
}

UOdysseyPainterEditorVectorPathPushTool*
FOdysseyPainterEditor::GetVectorPathPushTool() const
{
    return mVectorPathPushTool;
}

UOdysseyPainterEditorVectorPathSmoothTool*
FOdysseyPainterEditor::GetVectorPathSmoothTool() const
{
    return mVectorPathSmoothTool;
}

UOdysseyPainterEditorVectorPathStitchTool*
FOdysseyPainterEditor::GetVectorPathStitchTool() const
{
    return mVectorPathStitchTool;
}

UOdysseyPainterEditorVectorPaintBucketTool*
FOdysseyPainterEditor::GetVectorPaintBucketTool() const
{
    return mVectorPaintBucketTool;
}

UOdysseyPainterEditorColorPickerTool*
FOdysseyPainterEditor::GetColorPickerTool() const
{
    return mColorPickerTool;
}

UOdysseyPainterEditorAnimationOutOfPegsTool*
FOdysseyPainterEditor::GetOutOfPegsTool() const
{
    return mOutOfPegsTool;
}

UOdysseyPainterEditorColorPickerTool*
FOdysseyPainterEditor::GetTemporaryColorPickerTool() const
{
    return mTemporaryColorPickerTool;
}

TArray<FOdysseyBrushContext*>&
FOdysseyPainterEditor::GetBrushContexts()
{
    return mBrushContexts;
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyPainterEditor::HUDSystem() const
{
    return mHUDSystem;
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyPainterEditor::GetToolsHUD() const
{
    return mToolsHUD;
}

TSharedPtr<FOdysseyHUDElement>
FOdysseyPainterEditor::GetRasterSelectionHUD() const
{
    return mRasterSelectionHUD;
}

bool
FOdysseyPainterEditor::IsToolsHUDVisible() const
{
    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
        return true;

    return player->GetStatus() == EOdysseyAnimationPlayerStatus::Stopped;
}

bool
FOdysseyPainterEditor::IsRasterSelectionHUDVisible() const
{
    UOdysseyPainterEditorTool* tool = GetCurrentTool();
    if (!tool)
    {
        //Don't display Raster Selection when no tool is active
        return false;
    }

    if (!tool->UsesRasterSelection())
    {
        //Don't display Raster Selection if the current tool does not need it
        return false;
    }

    UOdysseyAnimationPlayer* player = GetAnimationPlayer();
    if (!player)
    {
        //Always display Raster Selection if there is no animation player
        //(eg. we are editing a texture)
        return true;
    }

    //Don't display Raster Selection if the animation is being played or scrubbed
    return player->GetStatus() == EOdysseyAnimationPlayerStatus::Stopped;
}

const FOdysseyBrushColor&
FOdysseyPainterEditor::PaintColor() const
{
    return mPaintColor;
}

UOdysseyPainterEditorTool* FOdysseyPainterEditor::GetEditorToolOfClass(UClass* iToolClass)
{
    // As there are now 2 color picker (the real tool and the temporary tool)
    // and they are of the same class UOdysseyPainterEditorColorPickerTool
    // just return nullptr when the there is a temporary tool activated
    if( mCurrentTemporaryTool )
        return nullptr;

    if( iToolClass == UOdysseyPainterEditorRasterDrawingTool::StaticClass() )
        return mRasterDrawingTool;
    else if( iToolClass == UOdysseyPainterEditorRasterEraserTool::StaticClass() )
        return mRasterEraserTool;
    else if( iToolClass == UOdysseyPainterEditorRasterSelectionTool::StaticClass() )
        return mRasterSelectionTool;
    else if( iToolClass == UOdysseyPainterEditorRasterTransformTool::StaticClass() )
        return mRasterTransformTool;
    else if( iToolClass == UOdysseyPainterEditorRasterPrimitiveDrawingTool::StaticClass() )
        return mRasterPrimitiveDrawingTool;
    else if (iToolClass == UOdysseyPainterEditorRasterLiquifyTool::StaticClass())
        return mRasterLiquifyTool;
    else if( iToolClass == UOdysseyPainterEditorRasterPaintBucketTool::StaticClass() )
        return mRasterPaintBucketTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPrimitiveDrawingTool::StaticClass() )
        return mVectorPrimitiveDrawingTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPathDrawingTool::StaticClass() )
        return mVectorPathDrawingTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPathEditTool::StaticClass() )
        return mVectorPathEditTool;
    else if( iToolClass == UOdysseyPainterEditorVectorSelectionTool::StaticClass() )
        return mVectorSelectionTool;
    else if( iToolClass == UOdysseyPainterEditorVectorCutTool::StaticClass() )
        return mVectorCutTool;
    else if( iToolClass == UOdysseyPainterEditorVectorScenePanTool::StaticClass() )
        return mVectorScenePanTool;
    else if( iToolClass == UOdysseyPainterEditorVectorEraserTool::StaticClass() )
        return mVectorEraserTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPathPushTool::StaticClass() )
        return mVectorPathPushTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPathSmoothTool::StaticClass() )
        return mVectorPathSmoothTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPathStitchTool::StaticClass() )
        return mVectorPathStitchTool;
    else if( iToolClass == UOdysseyPainterEditorVectorPaintBucketTool::StaticClass() )
        return mVectorPaintBucketTool;
    else if( iToolClass == UOdysseyPainterEditorColorPickerTool::StaticClass() )
        return mColorPickerTool;
    else if( iToolClass == UOdysseyPainterEditorVectorGridTool::StaticClass() )
        return mVectorGridTool;
    else if( iToolClass == UOdysseyPainterEditorVectorTransformTool::StaticClass() )
        return mVectorTransformTool;
    else if( iToolClass == UOdysseyPainterEditorVectorMatchingTool::StaticClass() )
        return mVectorMatchingTool;
    else if( iToolClass == UOdysseyPainterEditorVectorChartTool::StaticClass() )
        return mVectorChartTool;
    else if( iToolClass == UOdysseyPainterEditorVectorTrajectoryTool::StaticClass() )
        return mVectorTrajectoryTool;
    else
        return nullptr;
}

UOdysseyAnimation*
FOdysseyPainterEditor::GetAnimation() const
{
    if (!mSource || mSource->Id() != FOdysseyPainterEditorAnimationSource::StaticId() )
        return nullptr;


    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(mSource);
    return animationSource->GetAnimation();
}

UOdysseyTextureLayerStackUserData*
FOdysseyPainterEditor::GetTextureUserData() const
{
    if (!mSource || mSource->Id() != FOdysseyPainterEditorTextureSource::StaticId())
        return nullptr;

    TSharedPtr<FOdysseyPainterEditorTextureSource> textureSource = StaticCastSharedPtr<FOdysseyPainterEditorTextureSource>(mSource);
    return textureSource->TextureUserData();
}

UOdysseyAnimationPlayer*
FOdysseyPainterEditor::GetAnimationPlayer() const
{
    if (!mSource || mSource->Id() != FOdysseyPainterEditorAnimationSource::StaticId() )
        return nullptr;


    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(mSource);
    return animationSource->GetAnimationPlayer();
}

TSharedPtr<FOdysseyPainterEditorAnimationFlipSystem>
FOdysseyPainterEditor::GetAnimationFlipSystem() const
{
    return mAnimationFlipSystem;
}

EOdysseyPainterEditorColorType
FOdysseyPainterEditor::GetColorType() const
{
    return mColorType;
}

void
FOdysseyPainterEditor::SetColorType(EOdysseyPainterEditorColorType iType)
{
    mColorType = iType;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentMainTool() const
{
    return mCurrentMainTool;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentTemporaryTool() const
{
    return mCurrentTemporaryTool;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::GetCurrentTool() const
{
    return mCurrentTemporaryTool ? mCurrentTemporaryTool : mCurrentMainTool;
}

UOdysseyToolCollection* FOdysseyPainterEditor::GetRecentTools() const
{
    return mRecentTools;
}

void
FOdysseyPainterEditor::InactivateAllTools()
{
    if (!mCurrentMainTool && !mCurrentTemporaryTool)
        return;

    if (mCurrentTemporaryTool)
    {
        mToolsHUD->EmptyElements();
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }

    if (mCurrentMainTool)
    {
        mToolsHUD->EmptyElements();
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
        mOnCurrentMainToolChanged.Broadcast();
    }
    mOnCurrentToolChanged.Broadcast();

    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::InactivateMainTool()
{
    if (mCurrentMainTool)
    {
        mToolsHUD->EmptyElements();
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
        mOnCurrentMainToolChanged.Broadcast();
    }
    mOnCurrentToolChanged.Broadcast();
    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::ActivateMainTool( UOdysseyPainterEditorTool* iTool )
{
    if (mCurrentMainTool && mCurrentMainTool == iTool && mCurrentMainTool->IsActivated() )
        return;

    if (mCurrentTemporaryTool && mCurrentTemporaryTool->IsActivated())
    {
        mToolsHUD->EmptyElements();
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }

    if (mCurrentMainTool && mCurrentMainTool != iTool && mCurrentMainTool->IsActivated())
    {
        mToolsHUD->EmptyElements();
        mCurrentMainTool->Inactivate();
        mCurrentMainTool = nullptr;
    }

    if (!iTool || !iTool->IsActivable())
    {
        mOnCurrentMainToolChanged.Broadcast();
        mOnCurrentToolChanged.Broadcast();
        UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
        return;
    }

    mCurrentMainTool = iTool;
    mCurrentMainTool->Activate();
    mToolsHUD->AddElement(mCurrentMainTool->GetHUD());

    if (LayerStack())
    {
        UOdysseyLayer* currentLayer = LayerStack()->GetCurrentLayer();
        if (currentLayer)
        {
            UClass* layerClass = currentLayer->GetClass();
            if (!mCurrentMainToolPerLayerClass.Contains(layerClass))
                mCurrentMainToolPerLayerClass.Add(layerClass, nullptr);

            mCurrentMainToolPerLayerClass[layerClass] = mCurrentMainTool;
        }
    }

    mOnCurrentMainToolChanged.Broadcast();
    mOnCurrentToolChanged.Broadcast();
    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::InactivateTemporaryTool()
{
    if (!mCurrentTemporaryTool)
        return;

    mToolsHUD->EmptyElements();
    mCurrentTemporaryTool->Inactivate();
    mCurrentTemporaryTool = nullptr;
    mOnCurrentTemporaryToolChanged.Broadcast();

    if (mCurrentMainTool && mCurrentMainTool->IsActivable())
    {
        mCurrentMainTool->Activate();
        mToolsHUD->AddElement(mCurrentMainTool->GetHUD());
    }
    mOnCurrentToolChanged.Broadcast();
    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::ActivateTemporaryTool( UOdysseyPainterEditorTool* iTool )
{
    if (!iTool || mCurrentTemporaryTool == iTool || !iTool->IsActivable())
        return;

    if (mCurrentTemporaryTool && mCurrentTemporaryTool->IsActivated())
    {
        mToolsHUD->EmptyElements();
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
    }

    if (mCurrentMainTool && mCurrentMainTool->IsActivated())
    {
        mToolsHUD->EmptyElements();
        mCurrentMainTool->Inactivate();
    }

    mCurrentTemporaryTool = iTool;
    mCurrentTemporaryTool->Activate();
    mToolsHUD->AddElement(mCurrentTemporaryTool->GetHUD());

    mOnCurrentTemporaryToolChanged.Broadcast();
    mOnCurrentToolChanged.Broadcast();
    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::SanitizeCurrentTool()
{
    if (mCurrentTemporaryTool)
    {
        InactivateTemporaryTool();
        return;
    }

    if (mCurrentMainTool)
    {
        if (mCurrentMainTool->IsActivable())
        {
            mCurrentMainTool->Unload();
            mCurrentMainTool->Load();
            return;
        }
        InactivateMainTool();
    }

    UOdysseyLayerStack* layerStack = LayerStack();
    if (!layerStack)
        return;

    UOdysseyLayer* currentLayer = layerStack->GetCurrentLayer();
    if (!currentLayer)
        return;

    UOdysseyPainterEditorTool* tool = nullptr;
    UClass* layerClass = currentLayer->GetClass();
    if (mCurrentMainToolPerLayerClass.Contains(layerClass))
        tool = mCurrentMainToolPerLayerClass[layerClass];

    if (!tool || !tool->IsActivable())
        tool = FindDefaultToolForCurrentLayer();

    if (!tool || !tool->IsActivable())
        return;

    ActivateMainTool(tool);
}

FOdysseyMediaProvider
FOdysseyPainterEditor::GetCurrentMediaProvider()
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
    if (!source)
        return FOdysseyMediaProvider();

    return source->GetCurrentMediaProvider();
}

UOdysseyLayerStack*
FOdysseyPainterEditor::LayerStack() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
    if (!source)
        return nullptr;

    return source->GetLayerStack();
}

TObjectPtr<UOdysseyPainterEditorRasterSelection>
FOdysseyPainterEditor::RasterSelection()
{
    return mRasterSelection;
}

TSharedPtr<FOdysseyMeshSelector>
FOdysseyPainterEditor::GetMeshSelector() const
{
    return mMeshSelector;
}

int
FOdysseyPainterEditor::GetCurrentFrame() const
{
    if (!mSource)
        return INDEX_NONE;

    if (mSource->Id() == FOdysseyPainterEditorTextureSource::StaticId() )
        return 0;

    if (mSource->Id() == FOdysseyPainterEditorAnimationSource::StaticId() )
    {
        TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(mSource);
        UOdysseyAnimationPlayer* player = animationSource->GetAnimationPlayer();
        if (!player)
            return INDEX_NONE;

        return player->GetCurrentFrame().FrameNumber.Value;
    }

    return INDEX_NONE;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPainterEditor::SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource)
{
    if (mSource)
    {
        InactivateAllTools();

        mSource->Inactivate();
        mSource = nullptr;

        mRasterSelectionHUD->RemoveElement(mRasterSelection->GetHUD());
        mRasterSelection = NewObject< UOdysseyPainterEditorRasterSelection >();

        mAnimationTimelinePosition->Reset();
    }

    if (iSource && iSource->IsValid())
    {
        mSource = iSource;
        mSource->OnAddEditedObjectDelegate().AddLambda([this](UObject* iObject) { AddEditedObject(iObject);});
        mSource->OnRemoveEditedObjectDelegate().AddLambda([this](UObject* iObject) { RemoveEditedObject(iObject);});
        mSource->Activate(this);

        mRasterSelection->Init(mSource->Width(), mSource->Height());
        mRasterSelectionHUD->AddElement(mRasterSelection->GetHUD());

        if ( mCurrentMainTool && mCurrentMainTool->IsActivable() )
        {
            ActivateMainTool(mCurrentMainTool);
        }
        else
        {
            //select the best tool
            SanitizeCurrentTool();
        }

        if ( mSource->Id() == FOdysseyPainterEditorAnimationSource::StaticId() )
        {
            uint64 allowedFlags = FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED
                                | FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED
                                | FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED;
            uint64 editionFlags = GetVectorHUDFlags() & ( FOdysseyVectorHUD::HUD_MODE_OBJECT
                                                        | FOdysseyVectorHUD::HUD_MODE_VERTEX
                                                        | FOdysseyVectorHUD::HUD_MODE_INBETWEEN );

            SetVectorHUDFlags( editionFlags | allowedFlags );
        }

        if ( mSource->Id() == FOdysseyPainterEditorTextureSource::StaticId() )
        {
            uint64 allowedFlags = FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED
                                | FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED;
            uint64 editionFlags = GetVectorHUDFlags() & ( FOdysseyVectorHUD::HUD_MODE_OBJECT
                                                        | FOdysseyVectorHUD::HUD_MODE_VERTEX );

            SetVectorHUDFlags( editionFlags | allowedFlags );
        }
    }

    OnSourceChanged().Broadcast();
}

void
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit)
{
    mPaintColor = iColor;
    mCurrentPaletteEntryColor = nullptr;
    mCurrentPaletteSet = FGuid();

    //PATCH: should be automatic in the new drawing Tool, fix it asap
    if (iIsCommit)
    {
        GetRasterDrawingTool()->GetBrushOptions()->SetColor(iColor);
        if (GetCurrentTool() == GetRasterDrawingTool())
        {
            GetRasterDrawingTool()->GetBrushInstance()->ExecuteStateChanged();
        }
    }
}

TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition>
FOdysseyPainterEditor::GetAnimationTimelinePosition()
{
    return mAnimationTimelinePosition;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::FindDefaultToolForCurrentLayer()
{
    for (TObjectPtr<UOdysseyPainterEditorTool> tool : mMainTools)
    {
        if ( !tool->IsActivable() )
            continue;

        return tool;
    }
    return nullptr;
}

void
FOdysseyPainterEditor::AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension)
{
    mExtensions.Add(iExtension);
}

void
FOdysseyPainterEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
    if ( iLayerStack != LayerStack() )
        return;

    SanitizeCurrentTool(); //Refresh the current tool when we change layer
}

void
FOdysseyPainterEditor::SetVectorHUDFlags( uint64 iVectorHUDFlags )
{
    mVectorHUDFlags = iVectorHUDFlags;
}

uint64
FOdysseyPainterEditor::GetVectorHUDFlags()
{
    return mVectorHUDFlags;
}

void
FOdysseyPainterEditor::SetVectorDrawingFlags( uint64 iVectorDrawingFlags )
{
    mVectorDrawingFlags = iVectorDrawingFlags;
}

uint64
FOdysseyPainterEditor::GetVectorDrawingFlags()
{
    return mVectorDrawingFlags;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Callbacks

void
FOdysseyPainterEditor::OnApplyOverrides(const TMap<FName, UObject*>& iOverrides)
{
    //TODO: Apply Overrides for Paint Color
    //TODO: Apply Overrides for Other things like HUDs, Mesh Selector, or anything else
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyPainterEditor::Tick(float iDeltaTime)
{
    UOdysseyPainterEditorTool* tool = GetCurrentTool();
    if (tool)
        tool->Tick(iDeltaTime);
}

//--------------------------------------------------------------------------------------
//------ Generic methods for vector layers. currently placed here, although it's not really needed.
//------ This might be put somewhere else, in a CommonFunctions file or something as static methods.

void
FOdysseyPainterEditor::AddEditMenuEntry( FMenuBuilder& iMenuBuilder )
{
    UOdysseyPainterEditorVectorBaseTool* vectorBaseTool = Cast<UOdysseyPainterEditorVectorBaseTool>(GetCurrentTool());

    if( vectorBaseTool )
    {
        iMenuBuilder.BeginSection("Tool Options", TAttribute(FText::FromString("Tool/Options")));
        {
            vectorBaseTool->ExtendContextMenu( iMenuBuilder );
        }
        iMenuBuilder.EndSection();
    }
}

// static
void
FOdysseyPainterEditor::BringForward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.bring-forward", "Bring forward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        selectedObject->BringForward();

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }
}

// static
void
FOdysseyPainterEditor::SendBackward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.send-backward", "Send backward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        selectedObject->SendBackward();

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }
}

// static
void
FOdysseyPainterEditor::ApplyTransformations( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only top-most objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // Backup before, for undoing
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.apply-transformations", "Apply Transformations"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoApplyTransformations( iScene
                                                                             , objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* transformedObject : objectList )
    {
        transformedObject->ApplyTransformations();
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::MakePaintGroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorBucket*> removedBucketArray;
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroupPaint* paintGroup;

    // concerns all selected objects of a branch but the scene
    iScene->GetCell()->GetFocusedObjectList( objectList );

    paintGroup = FOdysseyVectorObject::MakePaintGroupFromObjects( iScene
                                                                , objectList
                                                                , cubicPathArray
                                                                , removedBucketArray );

    if( paintGroup )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.make-group-paint", "Group Paint"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , paintGroup
                                                                  , cubicPathArray
                                                                  , removedBucketArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        iScene->GetCell()->ClearObjectSelection();
        iScene->GetCell()->SelectObject( paintGroup );

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }
}

// static
void
FOdysseyPainterEditor::Ungroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

    if( selectedObject )
    {
        if( selectedObject->HasBaseClass( FOdysseyVectorGroup::StaticClass() ) )
        {
            FOdysseyVectorGroup* group = static_cast<FOdysseyVectorGroup*>( selectedObject );
            FOdysseyVectorObject* groupParent = group->GetParent();
            // we work on a copy of the list to be able to delete children while iterating
            std::list<FOdysseyVectorObject*> childrenList = group->GetChildrenList();

            // needed for undos
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.ungroup", "Ungroup"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoUngroup( iScene
                                                                        , group );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            for( FOdysseyVectorObject* child : childrenList )
            {
                groupParent->TransferChild( child, groupParent->GetLastChild() );
            }

            //groupParent->RemoveChild( group );

            iScene->GetCell()->ClearObjectSelection();

            iScene->UpdateMatrix();

            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
        }
    }
}

void
FOdysseyPainterEditor::GroupAndAddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                , FOdysseyVectorGroupPaint* iScene )
{
    // this call will also handle the creation of an Undo entry
    FOdysseyVectorGroup* group = _Group( iEditor, iScene );

    if( group )
    {
        uint32 numQuadX = 24, numquadY = 24;
        FOdysseyVectorTagInbetweener *inbetweenerTag = new FOdysseyVectorTagInbetweener( group
                                                                                       , numQuadX
                                                                                       , numquadY
                                                                                       , eInbetweenerGridType::ARAP );

        group->AddTag( inbetweenerTag );

        if( inbetweenerTag->GetTargetCell() == nullptr )
        {
            inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Backward );
        }
    }

    // note: updating via Root will request a redraw as well
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::Group( FOdysseyPainterEditor* iEditor
                            , FOdysseyVectorGroupPaint* iScene )
{
    _Group( iEditor, iScene );
}

// static
FOdysseyVectorGroup*
FOdysseyPainterEditor::_Group( FOdysseyPainterEditor* iEditor
                             , FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorObject*> objectOldParentArray;
    std::vector<FOdysseyVectorObject*> objectArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroup* group;

    iScene->GetCell()->GetFocusedAncestorList( objectList );

    group = FOdysseyVectorObject::GroupObjects( iScene, objectList, objectArray );

    if( group )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.group", "Group"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoGroup( iScene
                                                                  , group
                                                                  , objectArray );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        iScene->GetCell()->ClearObjectSelection();
        iScene->GetCell()->SelectObject( group );

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
    }

    return group;
}

// static
void
FOdysseyPainterEditor::Subdivide( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;

    // concerns all selected objects of a branch including implicit selection
    iScene->GetCell()->GetFocusedObjectList( objectList );

    for( FOdysseyVectorObject* focusedObject : objectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);

            path->Subdivide( addedVertexArray, addedSegmentArray, removedSegmentArray );
        }
    }

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.subdivide","Subdivide"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // request redraw
    //iScene->GetCell()->GetCellEngine()->Invalidate( iScene, 0 );
}

// static
void
FOdysseyPainterEditor::SelectAllPoints( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;

    // concerns all selected objects of a branch including implicit selection
    iScene->GetCell()->GetFocusedObjectList( objectList );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-points","Select All Points"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene
                                                                     , objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* focusedObject : objectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);

            path->SelectAllVertices();
        }

        if( focusedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(focusedObject);

            paintgroup->SelectAllBuckets();
        }
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::SelectAllObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-objects", "Select All"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene->GetLayer()
                                                                     , iScene->GetCell() );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetCell()->SelectAllInSelectionSpace();

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::ResetView( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-view", "Reset view"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->ResetTransform();
    iScene->UpdateMatrix();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::LockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene
                                                                   , selectedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetLocked( true );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::UnlockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene
                                                                   , selectedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetLocked( false );
    }

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::UnalignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene
                                                                        , selectedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    // the actual unalignment
    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        vertex->SetHandleAligned( false );
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::AlignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.align-point-selection","Align Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene
                                                                        , selectedVertexArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();
    //---------- end of undo ------------//

    // the actual alignment ( alignment is based on the first segment met)
    for( FOdysseyVectorVertex* vertex : selectedVertexArray )
    {
        FOdysseyVectorSegment* segment = vertex->GetFirstSegment();

        if( segment )
        {
            vertex->SetHandleAligned( true );
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::DeletePointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorPath*> removedPathArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorPath*> addedPathArray;
    std::vector<FOdysseyVectorVertex*> addedVertexArray; // not filled, here just for the undo record
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::list<FOdysseyVectorObject*> objectList;
    // concerns all selected objects of a branch including implicit selection
    iScene->GetCell()->GetFocusedObjectList( objectList );

    removedPathArray.reserve( 10 );
    removedVertexArray.reserve( 10 );
    removedSegmentArray.reserve( 10 );
    addedSegmentArray.reserve( 10 );

    for( FOdysseyVectorObject* focusedObject : objectList )
    {
        if( focusedObject->HasBaseClass( FOdysseyVectorPath::StaticClass() ) )
        {
            FOdysseyVectorPath* path = static_cast<FOdysseyVectorPath*>(focusedObject);
            std::vector<FOdysseyVectorVertex*> selectedVertexArray;

            path->GetSelectedVertices( selectedVertexArray );

            if( selectedVertexArray.size() )
            {
                FOdysseyVectorPath::DeleteVertex( path
                                                , selectedVertexArray
                                                , removedVertexArray
                                                , removedSegmentArray
                                                , removedPathArray
                                                , addedSegmentArray );
            }
        }
    }

    for( int i = 0; i < removedPathArray.size(); i++ )
    {
        FOdysseyVectorPath* path = removedPathArray[i];

        path->GetParent()->RemoveChild( path );
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // updated invalidated objects

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-point-selection","Delete Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoPathAlter( iScene
                                                                  , removedPathArray
                                                                  , removedVertexArray
                                                                  , removedSegmentArray
                                                                  , addedPathArray
                                                                  , addedVertexArray
                                                                  , addedSegmentArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::DeleteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorObject*> removedObjectArray;

    removedObjectArray.reserve( selectedObjectList.size() );

    iScene->GetCell()->RemoveObjects( selectedObjectList, removedObjectArray );

    iScene->GetCell()->ClearObjectSelection();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-objects", "Delete Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoRemoveObjects( iScene
                                                                      , removedObjectArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> removedTagArray;

    removedTagArray.reserve( selectedObjectList.size() );

    for( FOdysseyVectorObject* selectedObject : selectedObjectList )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            selectedObject->RemoveTag( tag );

            removedTagArray.push_back( tag );

            inbetweenerTag->RedrawCells();
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-tags", "Remove Tags"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagRemove( iScene
                                                                  , removedTagArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::CommitSelectedInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                   , FOdysseyVectorLayer* iLayer )
{
    std::list<FOdysseyVectorGroupPaint*> committedSceneList;
    std::list<FOdysseyVectorObject*> addedObjectList;
    std::list<FOdysseyVectorTag*> removedTagList;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iLayer->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass(), selectedTagList );


    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        // commit will concern the whole tree, we just commit the top one
        if( inbetweenerTag->IsTopSelectedTag() )
        {
            inbetweenerTag->Commit( removedTagList
                                  , addedObjectList
                                  , committedSceneList );
        }
    }

    if( removedTagList.size() )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.commit-tags", "Commit Tags"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerCommit( iLayer
                                                                                 , removedTagList
                                                                                 , addedObjectList
                                                                                 , committedSceneList );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        iLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iLayer->RequestRedraw( nullptr, 0 );
    }
}

// static
void
FOdysseyPainterEditor::AddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> addedTagArray;

    for( FOdysseyVectorObject* selectedObject : selectedObjectList )
    {
        FOdysseyVectorTag* tag = selectedObject->GetTagByType( FOdysseyVectorTagInbetweener::StaticClass() );

        if( tag == nullptr )
        {
            uint32 numQuadX = 24, numquadY = 24;
            FOdysseyVectorTagInbetweener* inbetweenerTag = new FOdysseyVectorTagInbetweener( selectedObject
                                                                                           , numQuadX
                                                                                           , numquadY
                                                                                           , eInbetweenerGridType::ARAP );

            selectedObject->AddTag( inbetweenerTag );

            if( inbetweenerTag->GetTargetCell() == nullptr )
            {
                inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Backward );
            }
            // turn again if there is no target cell (i.e there is a signle cell). Hence, default is forward.
            if( inbetweenerTag->GetTargetCell() == nullptr )
            {
                inbetweenerTag->SetInterpolationDirection( eInbetweenerInterpolationDirection::Forward );
            }

            addedTagArray.push_back( inbetweenerTag );
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.add-tags", "Add Tags"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagAdd( iScene
                                                               , addedTagArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

static std::vector<::ULIS::FVec2D>&
GetCopiedInbetweenerGridGeometry()
{
    static std::vector<::ULIS::FVec2D> inbetweenerGridGeometry;

    return inbetweenerGridGeometry;
}

static ::ULIS::FVec2I&
GetCopiedInbetweenerGridSize()
{
    static ::ULIS::FVec2I gridSize;

    return gridSize;
}

// static
void
FOdysseyPainterEditor::CopyInbetweenerGrid( FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                 , selectedTagList );

    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
        FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( iScene->GetCell()->GetIndex() );

        if( breakdown )
        {
            breakdown->GetGrid()->GetGeometry( GetCopiedInbetweenerGridGeometry()
                                             , eInbetweenerPointPositionType::TargetPosition );

            GetCopiedInbetweenerGridSize().x = inbetweenerTag->GetGridNumQuadX();
            GetCopiedInbetweenerGridSize().y = inbetweenerTag->GetGridNumQuadY();

            break;
        }
    }
}

// static
void
FOdysseyPainterEditor::PasteInbetweenerGrid( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTag*> selectedTagList;
    std::list<FInbetweenerBreakdown*> selectedBreakdownList;
    std::vector<::ULIS::FVec2D>& copiedGridGeometry = GetCopiedInbetweenerGridGeometry();

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                 , selectedTagList );

    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
        FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( iScene->GetCell()->GetIndex() );

        if( breakdown )
        {
            if( ( GetCopiedInbetweenerGridSize().x == inbetweenerTag->GetGridNumQuadX() )
             && ( GetCopiedInbetweenerGridSize().y == inbetweenerTag->GetGridNumQuadY() ) )
            {
                selectedBreakdownList.push_back( breakdown );
            }
        }
    }

    if( selectedBreakdownList.size() )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                               , selectedBreakdownList );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-grid-geometry","Paste Grid Geometry"));
        if( GUndo )
        {
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        undo->Begin(); // snpashot before changes

        for( FInbetweenerBreakdown* breakdown : selectedBreakdownList )
        {
            breakdown->GetGrid()->SetGeometry( copiedGridGeometry
                                             , eInbetweenerPointPositionType::TargetPosition
                                             , true );
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        undo->End(); // snpashot after changes
    }
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerGrid( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene
                                           , bool iResetTransformation
                                           , bool iResetDeformation )
{
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                 , selectedTagList );

    for( FOdysseyVectorTag* tag : selectedTagList )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

        selectedInbetweenerTagList.push_back( inbetweenerTag );
    }

    if( selectedInbetweenerTagList.size() )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerReset( iScene
                                                                            , selectedInbetweenerTagList
                                                                            , iResetDeformation
                                                                            , iResetTransformation );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-grid","Reset Inbetweener Grid"));
        if( GUndo )
        {
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        undo->Begin(); // snapshot before changes

        for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
        {
            FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( iScene->GetCell()->GetIndex() );

            if( breakdown )
            {
                if( iResetTransformation )
                {
                    breakdown->SetTargetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f );
                    breakdown->UpdateMatrix();
                }

                if( iResetDeformation )
                {
                    breakdown->GetGrid()->ResetDeformation( true );
                }
            }
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        undo->End(); // snapshot before changes
    }
}

// static
void
FOdysseyPainterEditor::ResetSpacingChart( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene
                                        , bool iResetPositionning
                                        , bool iCurrentBreakdownOnly )
{
    std::list<FOdysseyVectorTag*> selectedTagList;
    uint32 cellIndex = iScene->GetCell()->GetIndex();

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    if( selectedTagList.size() )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene->GetLayer()
                                                                                 , selectedTagList );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        undo->Begin(); // snapshot before changes

        for( FOdysseyVectorTag* tag : selectedTagList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            if( iCurrentBreakdownOnly )
            {
                FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( cellIndex );

                if( breakdown )
                {
                    breakdown->GetChart()->Reset( iResetPositionning );
                }
            }
            else
            {
                for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
                {
                    breakdown->GetChart()->Reset( iResetPositionning );
                }
            }
        }

        iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

        undo->End(); // snapshot after changes
    }
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( FOdysseyPainterEditor* iEditor
                                                      , FOdysseyVectorLayer* iLayer )
{
    std::list<FOdysseyVectorTag*> selectedTagList;

    iLayer->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                         , selectedTagList );

    if( selectedTagList.size() )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iLayer
                                                                                 , selectedTagList );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        undo->Begin(); // snapshot before changes

        for( FOdysseyVectorTag* tag : selectedTagList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
            {
                breakdown->GetChart()->Reset( false );
            }
        }

        // updated invalidated objects.
        // Updating via SharedEnv this will request a redraw as well
        iLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iLayer->RequestRedraw( nullptr, 0 );

        undo->End(); // snapshot after changes
    }


}

// static
void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only the top-most selected objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-horizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene
                                                                        , objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorObject::FlipObjectsHorizontal( objectList );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::FlipVertical( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;

    // concerns only the top-most selected objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-vertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene
                                                                        , objectList );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorObject::FlipObjectsVertical( objectList );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::ClearColoring( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorBucket*> bucketArray;
    std::list<FOdysseyVectorObject*> objectList;

    // concerns all objects of a branch, including the scene
    iScene->GetCell()->GetFocusedObjectList( objectList );

    bucketArray.reserve( 100 );

    // first step: retrieve all buckets for undoing.
    for( FOdysseyVectorObject* clearedObject : objectList )
    {
        if( clearedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(clearedObject);
            std::list<FOdysseyVectorBucket*>& bucketList = paintgroup->GetBucketList();

            for( FOdysseyVectorBucket* bucket : bucketList )
            {
                bucketArray.push_back( bucket );
            }
        }
    }

    // second step: the actual removal.
    for( FOdysseyVectorObject* clearedObject : objectList )
    {
        if( clearedObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
        {
            FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(clearedObject);

            paintgroup->RemoveAllBuckets();
        }
    }

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.clear-coloring", "Clear Coloring"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene
                                                                     , bucketArray );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::DeleteBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-bucket","Delete Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( scene
                                                                         , iBucket );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group
    scene->GetLayer()->RequestRedraw( nullptr, 0 );
}

void
FOdysseyPainterEditor::AlterContourWidth( FOdysseyVectorGroupPaint* iScene
                                        , double iValue
                                        , bool   iAbsolute )
{
    //std::list<FOdysseyVectorGroupPaint*> paintgroupList;

    FOdysseyVectorObject::Traverse
    ( iScene
    , 0
    , [ iScene
      //, &paintgroupList
      , iValue
      , iAbsolute ]( FOdysseyVectorObject* object, uint64 traversalFlags ) -> uint64
      {
          if( iScene->GetCell()->ObjectHasFocus( object, traversalFlags ) )
          {
              if( object->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
              {
                  FOdysseyVectorGroupPaint* paintgroup = static_cast<FOdysseyVectorGroupPaint*>(object);

                  //paintgroupList.push_back( paintgroup );

                  paintgroup->AlterContourWidth( iValue, iAbsolute );
              }
              // do not recurse
              return FOdysseyVectorObject::TRAVERSE_OBJECT_IGNORE_CHILDREN;
          }

          return 0;
      } );

    //for( )
    // request redraw
    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

bool FOdysseyPainterEditor::HasCopyBlockClipboard()
{
    FOdysseyCoreEditorModule& odysseyCoreEditorModule = FModuleManager::Get().LoadModuleChecked<FOdysseyCoreEditorModule>(TEXT("OdysseyCoreEditor"));

    return odysseyCoreEditorModule.GetClipboard()->GetId() == FOdysseyBlockClipboardData::StaticId();
}

static void
SetBucketPropagation( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket, bool iPropagate )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.propagate-bucket","Propagate Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( scene
                                                                    , iBucket );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::PropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iEditor, iBucket, true );
}

// static
void
FOdysseyPainterEditor::UnpropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    SetBucketPropagation( iEditor, iBucket, false );
}

static std::list<std::vector<float>>&
GetCopiedChart()
{
    static std::list<std::vector<float>> spacingList;

    return spacingList;
}

// static
void
FOdysseyPainterEditor::PasteSpacingChart( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene
                                        , bool iCurrentBreakdownOnly )
{
    std::list<std::vector<float>>& spacingList = GetCopiedChart();
    uint32 cellIndex = iScene->GetCell()->GetIndex();
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                 , selectedTagList );

    if( spacingList.size() > 0 )
    {
        if( selectedTagList.size() )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene->GetLayer()
                                                                                     , selectedTagList );

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-chart","Paste Spacing Chart"));
            if( GUndo )
            {
                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

            undo->Begin(); // snapshot before changes

            for( FOdysseyVectorTag* tag : selectedTagList )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                if( iCurrentBreakdownOnly )
                {
                    FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( cellIndex );

                    if( breakdown )
                    {
                        for( std::vector<float>& spacingBuffer : spacingList )
                        {
                            if( breakdown->GetDrawingCount() == spacingBuffer.size() )
                            {
                                for( uint32 j = 0; j < spacingBuffer.size(); j++ )
                                {
                                    breakdown->GetChart()->GetInbetweenBuffer()[j].SetSpacing( spacingBuffer[j] );
                                }
                            }
                        }
                    }
                }
                else
                {
                    std::vector<FInbetweenerBreakdown*> breakdownArray;
                    uint32 i = 0;

                    inbetweenerTag->GetBreakdownArray( breakdownArray );

                    for( std::vector<float>& spacingBuffer : spacingList )
                    {
                        if( i < breakdownArray.size() )
                        {
                            FInbetweenerBreakdown* breakdown = breakdownArray[i];

                            if( breakdown->GetDrawingCount() == spacingBuffer.size() )
                            {
                                for( uint32 j = 0; j < spacingBuffer.size(); j++ )
                                {
                                    breakdown->GetChart()->GetInbetweenBuffer()[j].SetSpacing( spacingBuffer[j] );
                                }
                            }
                        }

                        i++;
                    }
                }
            }

            iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
            iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

            undo->End(); // snapshot after changes
        }
    }


}

// static
void
FOdysseyPainterEditor::CopySpacingChart( FOdysseyPainterEditor* iEditor
                                       , FOdysseyVectorGroupPaint* iScene
                                       , bool iCurrentBreakdownOnly )
{
    std::list<std::vector<float>>& spacingList = GetCopiedChart();
    std::list<FOdysseyVectorTag*> selectedTagList;
    uint32 cellIndex = iScene->GetCell()->GetIndex();

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    if( selectedTagList.size() )
    {
        FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(selectedTagList.front());

        spacingList.clear();

        if( iCurrentBreakdownOnly )
        {
            FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( cellIndex );

            if( breakdown )
            {
                std::vector<float> spacingBuffer;

                breakdown->GetChart()->GetSpacing( spacingBuffer );

                spacingList.push_back( spacingBuffer );
            }
        }
        else
        {
            for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
            {
                std::vector<float> spacingBuffer;

                breakdown->GetChart()->GetSpacing( spacingBuffer );

                spacingList.push_back( spacingBuffer );
            }
        }
    }
}

static std::list<FOdysseyVectorObject*>&
GetCopiedObjectList()
{
    static std::list<FOdysseyVectorObject*> copiedObjectList;

    return copiedObjectList;
}

static FOdysseyVectorObject*
GetStoreObject()
{
    static FOdysseyVectorObject storeObject("StoreObject");

    return &storeObject;
}

// static
void
FOdysseyPainterEditor::CopyTransformation( FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

    if( selectedObject )
    {
        FOdysseyVectorObject* storeObject = GetStoreObject();

        selectedObject->CopyTransformation( *storeObject );
    }
}

// static
void
FOdysseyPainterEditor::PasteTransformation( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();

    if( selectedObject )
    {
        FOdysseyVectorObject* storeObject = GetStoreObject();

        //----- needed for undos -----//
        GEditor->BeginTransaction(LOCTEXT("PasteTransformation", "Paste Transformation"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, selectedObject );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
        // -------------------------- //

        storeObject->CopyTransformation( *selectedObject );

        selectedObject->UpdateMatrix();
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::CopyObjects( FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;

    // concerns all objects of a branch but the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    if( objectList.size() )
    {
        // First step : clear previously copied objects
        GetCopiedObjectList().remove_if( []( FOdysseyVectorObject* iCopiedObject ){ delete iCopiedObject; return true; } );

        // second step : copy selection.
        for( FOdysseyVectorObject* copiedObject : objectList )
        {
            GetCopiedObjectList().push_back( copiedObject->Copy() );
        }
    }
}

// static
void
FOdysseyPainterEditor::PasteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> pastedObjectList;

    // TODO: Check why pastedObjectList has to be copied, unclear
    // First copy all objects. This is needed to record their state-before-addition for the UNDO operation.
    for( FOdysseyVectorObject* copiedObject : GetCopiedObjectList() )
    {
        pastedObjectList.push_back( copiedObject->Copy() );
    }

    iScene->GetCell()->ClearObjectSelection();

    // This undo must be set before association with the new parent object
    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-objects","Paste"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = static_cast<FOdysseyVectorUndo*>( new FOdysseyVectorUndoObjectAdd( iScene
                                                                                                    , pastedObjectList ) );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    for( FOdysseyVectorObject* pastedObject : pastedObjectList )
    {
        //BLPoint shifting;

        iScene->AppendChild( pastedObject );

        //shifting = iScene->GetInverseWorldMatrix().mapVector( 10.0f, 10.0f ); // shift object by 10 pixels

        //pastedObject->Invalidate();
        //pastedObject->Translate( newObject->GetTranslationX() + shifting.x, newObject->GetTranslationY() + shifting.y );
        pastedObject->UpdateMatrix();

        iScene->GetCell()->SelectObject( pastedObject );
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                                  , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray )
{
    for( int i = 0; i < iSourceSceneArray.Num(); i++ )
    {
        for( FOdysseyVectorObject* child : iSourceSceneArray[i]->GetChildrenList() )
        {
            FOdysseyVectorObject* copiedChild = child->Copy();

            iDestinationScene->AppendChild( copiedChild );
        }

        iSourceSceneArray[i]->CopyBuckets( iDestinationScene, false );
    }

    iDestinationScene->UpdateMatrix();
    iDestinationScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iDestinationScene->GetLayer()->RequestRedraw( iDestinationScene->GetCell(), 0 );
}

// static
void
FOdysseyPainterEditor::RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorLayer* iLayer )
{
    std::list<FOdysseyVectorTag*> tagList;

    iLayer->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                     , tagList );

    if( tagList.size() )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-tags", "Remove Tags"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagRemove( tagList.front()->GetOwner()->GetScene()
                                                                      , tagList );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            if (iEditor)
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTag* tag : tagList )
        {
            tag->GetOwner()->RemoveTag( tag );
            tag->GetOwner()->GetCell()->UnselectObject( tag->GetOwner() );
        }
    }

    iLayer->RequestRedraw( nullptr, 0 );

    // update UI
    iLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
}

void
FOdysseyPainterEditor::StitchVertices( FOdysseyPainterEditor* iEditor
                                     , FOdysseyVectorGroupPaint* iScene
                                     , FOdysseyVectorVertex* iVertexA
                                     , FOdysseyVectorVertex* iVertexB )
{
    FOdysseyVectorVertex* StitchVertex;
    // for undos
    std::vector<FOdysseyVectorPath*> addedPathArray; // stays empty
    std::vector<FOdysseyVectorVertex*> addedVertexArray;
    std::vector<FOdysseyVectorSegment*> addedSegmentArray;
    std::vector<FOdysseyVectorPath*> removedPathArray; // receives the merged path if any
    std::vector<FOdysseyVectorSegment*> removedSegmentArray;
    std::vector<FOdysseyVectorVertex*> removedVertexArray;
    std::vector<FOdysseyVectorSegment*> mergedSegmentArray;
    std::vector<FOdysseyVectorVertex*> mergedVertexArray;
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    FOdysseyVectorPath* mergedPath = nullptr;

    if( ( iVertexA->GetSegmentCount() == 1 ) && ( iVertexB->GetSegmentCount() == 1 ) )
    {
        if( iVertexA->GetOwnerAsPath() != iVertexB->GetOwnerAsPath() )
        {
            // TODO: remove vertexB->GetPath() from selected objects.
            mergedPath = iVertexB->GetOwnerAsPath();

            iVertexB->GetOwnerAsPath()->GetParent()->RemoveChild( mergedPath );
            iVertexA->GetOwnerAsPath()->Merge( mergedPath, mergedVertexArray, mergedSegmentArray );
            // update the pointer with the newly created vertex's. Note, Merge alters the original vertex's ID.
            iVertexB = mergedVertexArray[iVertexB->GetID()];

            iScene->GetCell()->UnselectObject( mergedPath );

            removedPathArray.push_back( mergedPath );
        }

        StitchVertex = iScene->GetCell()->Stitch( iVertexA, iVertexB, addedSegmentArray, removedSegmentArray, true );

        if( StitchVertex )
        {
            addedVertexArray.push_back( StitchVertex );
            removedVertexArray.push_back( iVertexA );
            removedVertexArray.push_back( iVertexB );

            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.stitch-vertices","Vector Path Stitch Tool"));
            if( GUndo )
            {
                FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPathStitch( iScene
                                                                            , removedPathArray
                                                                            , removedVertexArray
                                                                            , removedSegmentArray
                                                                            , addedPathArray
                                                                            , addedVertexArray
                                                                            , addedSegmentArray
                                                                            , mergedVertexArray
                                                                            , mergedSegmentArray );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

FString
FOdysseyPainterEditor::GetReferencerName() const
{
    return "FOdysseyPainterEditor";
}

void
FOdysseyPainterEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
    for (TObjectPtr<UOdysseyPainterEditorTool> tool : mMainTools)
    {
        Collector.AddReferencedObject(tool);
    }

    for (TObjectPtr<UOdysseyPainterEditorTool> tool : mTemporaryTools)
    {
        Collector.AddReferencedObject(tool);
    }

    Collector.AddReferencedObject(mCurrentPaletteEntryColor);
    Collector.AddReferencedObject(mRecentTools);
    Collector.AddReferencedObject(mRasterSelection);
}

const TArray<UOdysseyPaletteSet*>
FOdysseyPainterEditor::GetPaletteSets() const
{
    TArray<UOdysseyPaletteSet*> paletteSets;
    UOdysseyAnimation* animation = GetAnimation();
    UOdysseyTextureLayerStackUserData* textureUserData = GetTextureUserData();
    if (animation)
    {
        paletteSets = animation->Palettes;
    }
    else if (textureUserData)
    {
        paletteSets = textureUserData->Palettes;
    }

    //Fail safe in case the user force delete a used palette while in the editor
    for( int i = 0; i < paletteSets.Num(); i++ )
    {
        if( !paletteSets[i]->mPalette || !paletteSets[i]->mPalette->IsValidLowLevel() )
        {
            paletteSets.RemoveAt(i);
            i--;
        }
    }

    // Legacy, to remove next version, ensure that the Palettes are going to be saved with the upgraded data
    for(int i = 0; i < paletteSets.Num(); i++)
    {
        if(paletteSets[i]->mPalette->NeedsSavingAfterUpgrade)
        {
            paletteSets[i]->mPalette->MarkPackageDirty();
            paletteSets[i]->mPalette->NeedsSavingAfterUpgrade = false;
        }
    }
    //---

    return paletteSets;
}

UOdysseyPaletteEntryColor*
FOdysseyPainterEditor::GetCurrentPaletteColorEntry() const
{
    return mCurrentPaletteEntryColor;
}

FGuid
FOdysseyPainterEditor::GetCurrentPaletteSet() const
{
    return mCurrentPaletteSet;
}

void
FOdysseyPainterEditor::AddPaletteSet(UOdysseyPalette* iPalette)
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
    if (!source)
        return;

    UOdysseyAnimation* animation = GetAnimation();
    UOdysseyTextureLayerStackUserData* textureUserData = GetTextureUserData();
    if (animation)
    {
        UOdysseyPaletteSet* paletteSet = NewObject<UOdysseyPaletteSet>(animation);
        paletteSet->mPalette = iPalette;
        paletteSet->mSet = iPalette->GetDefaultSetID();
        animation->Palettes.Add(paletteSet);
    }
    else if (textureUserData)
    {
        UOdysseyPaletteSet* paletteSet = NewObject<UOdysseyPaletteSet>(textureUserData);
        paletteSet->mPalette = iPalette;
        paletteSet->mSet = iPalette->GetDefaultSetID();
        textureUserData->Palettes.Add(paletteSet);
    }
}

void
FOdysseyPainterEditor::RemovePaletteSet(UOdysseyPaletteSet* iPaletteSet)
{
    UOdysseyAnimation* animation = GetAnimation();
    UOdysseyTextureLayerStackUserData* textureUserData = GetTextureUserData();
    TArray<UOdysseyPaletteSet*> paletteSets;
    if (animation)
    {
        animation->Palettes.Remove(iPaletteSet);
        paletteSets = animation->Palettes;
    }
    else if (textureUserData)
    {
        textureUserData->Palettes.Remove(iPaletteSet);
        paletteSets = textureUserData->Palettes;
    }

    if (mCurrentPaletteEntryColor)
    {
        bool shouldReset = !paletteSets.ContainsByPredicate(
            [this](const UOdysseyPaletteSet* iPaletteSet)
            {
                return iPaletteSet->mPalette == mCurrentPaletteEntryColor->GetPalette() && iPaletteSet->mSet == mCurrentPaletteSet;
            }
        );
        if (shouldReset)
        {
            mCurrentPaletteEntryColor = nullptr;
            mCurrentPaletteSet = FGuid();
        }
    }
}

void FOdysseyPainterEditor::SetPaletteSet(FGuid iIndex, UOdysseyPaletteSet* iPaletteSet)
{
    iPaletteSet->mSet = iIndex;

    //Update all vector objects
    UOdysseyAnimation* animation = GetAnimation();
    UOdysseyTextureLayerStackUserData* textureUserData = GetTextureUserData();

    if( animation )
    {
        TArray<UOdysseyLayer*> layers = LayerStack()->GetLayers();
        for (UOdysseyLayer* layer : layers)
        {
            for (UOdysseyLayerCell* cell : layer->GetCells())
            {
                UOdysseyAnimationCellImageVector* animationVectorCell;
                if (cell->IsA(UOdysseyAnimationCellImageVector::StaticClass()))
                    animationVectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
                else
                    continue;

                TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = animationVectorCell->GetMediaProvider().GetMedias<FOdysseyMediaVector>();

                if (mediaVectors.Num() > 0)
                {
                    for (int i = 0; i < mediaVectors.Num(); i++)
                    {
/*
                        FOdysseyVectorGroupPaint* vectorScene = mediaVectors[i]->GetScene();
                        std::list<FOdysseyVectorBucket*>& bucketList = vectorScene->GetBucketList();
                        std::list<FOdysseyVectorPath*>& pathList = vectorScene->GetPathList();
                        for (FOdysseyVectorBucket* bucket : bucketList)
                        {
                            if (bucket->GetPaletteEntry() && bucket->GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                                bucket->SetPaletteSetID(iIndex);
                        }
                        for (FOdysseyVectorPath* path : pathList)
                        {
                            if (path->GetBackgroundBucket().GetPaletteEntry() && path->GetBackgroundBucket().GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                                path->GetBackgroundBucket().SetPaletteSetID(iIndex);
                            if (path->GetForegroundBucket().GetPaletteEntry() && path->GetForegroundBucket().GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                                path->GetForegroundBucket().SetPaletteSetID(iIndex);
                        }
*/
                        FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();

                        vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                    }
                }
            }
        }
    }
    else if (textureUserData)
    {
        TArray<UOdysseyLayer*> layers = LayerStack()->GetLayers();
        for (UOdysseyLayer* layer : layers)
        {
            if (!layer->IsA(UOdysseyTextureLayerImageVector::StaticClass()))
                continue;

            UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(layer);

            TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = vectorLayer->GetMediaProvider(0).GetMedias<FOdysseyMediaVector>();

            if (mediaVectors.Num() > 0)
            {
                for (int i = 0; i < mediaVectors.Num(); i++)
                {
/*
                    FOdysseyVectorGroupPaint* vectorScene = mediaVectors[i]->GetScene();
                    std::list<FOdysseyVectorBucket*>& bucketList = vectorScene->GetBucketList();
                    std::list<FOdysseyVectorPath*>& pathList = vectorScene->GetPathList();
                    for (FOdysseyVectorBucket* bucket : bucketList)
                    {
                        if (bucket->GetPaletteEntry() && bucket->GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                            bucket->SetPaletteSetID(iIndex);
                    }
                    for (FOdysseyVectorPath* path : pathList)
                    {
                        if (path->GetBackgroundBucket().GetPaletteEntry() && path->GetBackgroundBucket().GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                            path->GetBackgroundBucket().SetPaletteSetID(iIndex);
                        if (path->GetForegroundBucket().GetPaletteEntry() && path->GetForegroundBucket().GetPaletteEntry()->GetPalette() == iPaletteSet->mPalette)
                            path->GetForegroundBucket().SetPaletteSetID(iIndex);
                    }
*/
                    FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
                    vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                }
            }
        }
    }
}

void
FOdysseyPainterEditor::SetCurrentPaletteColorEntry(UOdysseyPaletteEntryColor* iEntry, FGuid iSet)
{
    if (!iEntry)
    {
        mCurrentPaletteEntryColor = nullptr;
        mCurrentPaletteSet = FGuid();
        return;
    }

    UOdysseyPalette* palette = iEntry->GetPalette();
    if(palette && palette->GetSetsIDs().Contains(iSet))
    {
        FColor color = iEntry->GetColor(iSet);
        ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);

        mPaintColor = ulisColor;

        //PATCH: should be automatic in the new drawing Tool, fix it asap
        GetRasterDrawingTool()->GetBrushOptions()->SetColor(mPaintColor);
        if (GetCurrentTool() == GetRasterDrawingTool())
        {
            GetRasterDrawingTool()->GetBrushInstance()->ExecuteStateChanged();
        }
    }

    mCurrentPaletteEntryColor = iEntry;
    mCurrentPaletteSet = iSet;

    UOdysseyAnimation* animation = GetAnimation();
    UOdysseyTextureLayerStackUserData* textureUserData = GetTextureUserData();
    TArray<UOdysseyPaletteSet*> paletteSets;
    if (animation)
    {
        paletteSets = animation->Palettes;
    }
    else if (textureUserData)
    {
        paletteSets = textureUserData->Palettes;
    }

    for( int i = 0; i < paletteSets.Num(); i++)
    {
        if (paletteSets[i]->mPalette == palette)
            paletteSets[i]->mSet = iSet;
    }

    //Update all vector objects
    if (animation)
    {
        TArray<UOdysseyLayer*> layers = LayerStack()->GetLayers();
        for (UOdysseyLayer* layer : layers)
        {
            for (UOdysseyLayerCell* cell : layer->GetCells())
            {
                UOdysseyAnimationCellImageVector* animationVectorCell;
                if (cell->IsA(UOdysseyAnimationCellImageVector::StaticClass()))
                    animationVectorCell = Cast<UOdysseyAnimationCellImageVector>(cell);
                else
                    continue;

                TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = animationVectorCell->GetMediaProvider().GetMedias<FOdysseyMediaVector>();

                if (mediaVectors.Num() > 0)
                {
                    for (int i = 0; i < mediaVectors.Num(); i++)
                    {
                        FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
                        vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                    }
                }
            }
        }
    }
    else if (textureUserData)
    {
        TArray<UOdysseyLayer*> layers = LayerStack()->GetLayers();
        for (UOdysseyLayer* layer : layers)
        {
            if( !layer->IsA(UOdysseyTextureLayerImageVector::StaticClass()) )
                continue;

            UOdysseyTextureLayerImageVector* vectorLayer = Cast<UOdysseyTextureLayerImageVector>(layer);

            TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = vectorLayer->GetMediaProvider(0).GetMedias<FOdysseyMediaVector>();

            if (mediaVectors.Num() > 0)
            {
                for (int i = 0; i < mediaVectors.Num(); i++)
                {
                    FOdysseyVectorCell* vectorCell = mediaVectors[i]->GetScene()->GetCell();
                    vectorCell->GetLayer()->RequestRedraw(vectorCell, 0);
                }
            }
        }
    }
}

void FOdysseyPainterEditor::SaveMainToolToRecentTools()
{
    if (GetCurrentTool() != mCurrentMainTool)
        return;

    if( mRecentTools->ContainsSimilarToolConfiguration( mCurrentMainTool->GetClass(), mCurrentMainTool ) )
        return;

    TObjectPtr<UOdysseyPainterEditorTool> toolSnapshot;
    toolSnapshot = DuplicateObject< UOdysseyPainterEditorTool >(mCurrentMainTool, mRecentTools);

    FIconToolConfiguration iconToolConfig;
    iconToolConfig.mIconSource = EToolIconSource::Style;
    iconToolConfig.mIconStyleSet = mCurrentMainTool->mIconStyleSet;

    mRecentTools->AddToolConfiguration( mCurrentMainTool->GetClass(), toolSnapshot, iconToolConfig );

    if( mRecentTools->GetToolConfigurations().Num() > 10 )
        mRecentTools->RemoveToolConfigurationAtIndex( 0 );
}

#undef LOCTEXT_NAMESPACE
