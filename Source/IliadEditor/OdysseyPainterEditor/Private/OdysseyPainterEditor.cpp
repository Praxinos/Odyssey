// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorBrushContext.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "ObjectEditorUtils.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyPainterEditorAnimationFlipSystem.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyBlockClipboardData.h"
#include "OdysseyCoreEditorModule.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyHUD.h"
#include "OdysseyHUDElement.h"
#include "OdysseyMediaRaster.h"
#include "ULISLoaderModule.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyPainterEditorExtension.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyEditorLayoutBuilder.h"
#include "OdysseyPainterEditorBrushContext.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyPalette.h"
#include "OdysseyPaletteEntryColor.h"
#include "Proxies/OdysseyBrushColor.h"
#include "PaperFlipbook.h"
#include "PaperSprite.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "OdysseyPainterEditorFlipbookListener.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "OdysseyAnimationCurrentFrameMutator.h"

#include "OdysseyVector.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorTagInbetweener.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorObject.h"
#include "HUD/OdysseyVectorHUD.h"
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

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "Tools/RasterEraserTool/OdysseyPainterEditorRasterEraserTool.h"
#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "Tools/RasterTransformTool/OdysseyPainterEditorRasterTransformTool.h"
#include "Tools/RasterPrimitiveDrawingTool/OdysseyPainterEditorRasterPrimitiveDrawingTool.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"
#include "Tools/VectorPrimitiveDrawingTool/OdysseyPainterEditorVectorPrimitiveDrawingTool.h"
#include "Tools/VectorPathDrawingTool/OdysseyPainterEditorVectorPathDrawingTool.h"
#include "Tools/VectorPathEditTool/OdysseyPainterEditorVectorPathEditTool.h"
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionTool.h"
#include "Tools/VectorCutTool/OdysseyPainterEditorVectorCutTool.h"
#include "Tools/VectorScenePanTool/OdysseyPainterEditorVectorScenePanTool.h"
#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "Tools/VectorPathPushTool/OdysseyPainterEditorVectorPathPushTool.h"
#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "Tools/VectorPathStitchTool/OdysseyPainterEditorVectorPathStitchTool.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"
#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "Tools/VectorTransformTool/OdysseyPainterEditorVectorTransformTool.h"
#include "Tools/VectorMatchingTool/OdysseyPainterEditorVectorMatchingTool.h"
#include "Tools/VectorChartTool/OdysseyPainterEditorVectorChartTool.h"
#include "Tools/VectorTrajectoryTool/OdysseyPainterEditorVectorTrajectoryTool.h"
#include "Tools/OutOfPegsTool/OdysseyPainterEditorAnimationOutOfPegsTool.h"

#include "Shortcuts/OdysseyLayerStackGlobalShortcuts.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalShortcuts.h"
#include "Shortcuts/Global/OdysseyPainterEditorGlobalToolsShortcuts.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"
#include "Mesh/FOdysseyMeshSelector.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "Toolkits/BaseToolkit.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPainterEditorToolMenuContext.h"
#include "FileHelpers.h"
#include "OdysseyPainterEditorPaletteSet.h"
#include "Toolkits/AssetEditorModeUILayer.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

TSharedPtr<::ULIS::FBlock> FOdysseyPainterEditor::mCopyBlock = nullptr;

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{

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
    , mHUDSystem(new FOdysseyHUD())
    , mRasterSelection(MakeShared< FOdysseyPainterEditorRasterSelection >())
    , mBrushContexts()
    , mPaintColor(::ULIS::FColor::Black)
    , mAnimationTimelinePosition(MakeShared<FOdysseyPainterEditorAnimationTimelinePosition>())
    , mRasterDrawingTool(nullptr)
    , mRasterEraserTool(nullptr)
    , mRasterSelectionTool(nullptr)
    , mRasterTransformTool(nullptr)
    , mRasterPrimitiveDrawingTool(nullptr)
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
    , mAnimationFlipSystem(MakeShared<FOdysseyPainterEditorAnimationFlipSystem>(this))
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyPainterEditor::OnCurrentLayerChanged);

    //OnCurrentLayerChanged
    //OnCurrentFrameChanged
    //OnRenderingChanged

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

    //Init Tools
    InitTools();

    //Init the GUI
    mGUI = MakeShareable(new FOdysseyPainterEditorGUI(this));
    mGUI->Initialize();

    //Init the shortcuts

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

    GetShortcuts().Add(MakeShared<FOdysseyLayerStackGlobalShortcuts>(layerStack));
    GetShortcuts().Add(MakeShared<FOdysseyPainterEditorGlobalToolsShortcuts>(this));
    GetShortcuts().Add(MakeShared<FOdysseyPainterEditorGlobalShortcuts>(this));
    GetShortcuts().Add(MakeShared<FOdysseyAnimationGlobalShortcuts>(animation, currentFrame, onTransactCurrentFrame));

    SetVectorHUDFlags( FOdysseyVectorHUD::HUD_MODE_OBJECT
        | FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED
        | FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED
        | FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED );

    FSlateApplication::Get().RegisterInputPreProcessor(mAnimationFlipSystem);

    //Init the extensions
    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Initialize();
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

bool
FOdysseyPainterEditor::OnCloseRequested()
{
    return true;
}

void
FOdysseyPainterEditor::AddTab(TSharedRef<FOdysseyEditorTab> iTab)
{
    mTabs.Add(iTab);
}

void
FOdysseyPainterEditor::InitTabs()
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->Init();
    }
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

TSharedRef<FTabManager::FLayout>
FOdysseyPainterEditor::CreateLayout(const FName& iLayoutName)
{
    FOdysseyEditorLayoutBuilder builder(iLayoutName);
    mGUI->BuildLayout(builder);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->BuildLayout(builder);

    return builder.GetLayout();
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

    GetGUI()->BindShortcuts(iToolkit);

    for (UOdysseyPainterEditorTool* tool : mTools)
    {
        tool->BindShortcuts(iToolkit);
    }

    //---

    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateRaw( this, &FOdysseyPainterEditor::__VA_ARGS__ ), FCanExecuteAction() );
    #undef MAP_ACTION

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->BindShortcuts(iToolkit);
}

void
FOdysseyPainterEditor::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        tab->ExtendMenu( iExtender );
    }

    for (UOdysseyPainterEditorTool* tool : mTools)
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
        FNewToolBarDelegateLegacy::CreateLambda(
            [this](FToolBarBuilder& iBuilder, UToolMenu* iToolMenu)
            {
                ExtendToolbarSaveAssetButton(iBuilder);
                ExtendToolbarToolParameters(iBuilder);
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
        FNewToolBarDelegateLegacy::CreateLambda(
            [](FToolBarBuilder& iBuilder, UToolMenu* iToolMenu)
            {
                const UOdysseyPainterEditorToolMenuContext* Context = iToolMenu->FindContext<UOdysseyPainterEditorToolMenuContext>();
                if (!Context)
                    return;

                FOdysseyPainterEditor* painterEditor = Context->PainterEditor;
                if (!painterEditor)
                    return;

                painterEditor->ExtendToolbarToolParameters(iBuilder);
            }
        )
    );
}

void
FOdysseyPainterEditor::ExtendToolbarSaveAssetButton(FToolBarBuilder& iBuilder)
{
    FButtonArgs saveAssetButtonArgs;
    saveAssetButtonArgs.Command = FGenericCommands::Get().Undo;
    saveAssetButtonArgs.ToolTipOverride = LOCTEXT("top-tab.save-asset", "Saves the painted asset");
    saveAssetButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Save32");
    saveAssetButtonArgs.ExtensionHook = "Save";
    saveAssetButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::Button;
    saveAssetButtonArgs.Action = FUIAction(
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
    );

    iBuilder.BeginSection("Asset");
        iBuilder.AddToolBarButton(saveAssetButtonArgs);
    iBuilder.EndSection();
}

void
FOdysseyPainterEditor::ExtendToolbarToolParameters(FToolBarBuilder& iBuilder)
{
    FButtonArgs undoButtonArgs;
    undoButtonArgs.Command = FGenericCommands::Get().Undo;
    undoButtonArgs.ToolTipOverride = LOCTEXT("top-tab.undo", "Undo the previous action.");
    undoButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Undo32");
    undoButtonArgs.ExtensionHook = "Undo";
    undoButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::Button;
    undoButtonArgs.Action = FUIAction(
        FExecuteAction::CreateLambda(
            [this]()
            {
                if( GetCurrentMainTool() && GetCurrentMainTool()->IsA( UOdysseyPainterEditorRasterTransformTool::StaticClass() ) )
                    Cast<UOdysseyPainterEditorRasterTransformTool>( GetCurrentMainTool() )->UndoTransformTransaction();
                else
                    GEditor->UndoTransaction(true);
            }
        )
    );

    FButtonArgs redoButtonArgs;
    redoButtonArgs.Command = FGenericCommands::Get().Redo;
    redoButtonArgs.ToolTipOverride = LOCTEXT("top-tab.redo", "Redo the next action.");
    redoButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Redo32");
    redoButtonArgs.ExtensionHook = "Redo";
    redoButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::Button;
    redoButtonArgs.Action = FUIAction(
        FExecuteAction::CreateLambda(
            [this]()
            {
                if (GetCurrentMainTool() && GetCurrentMainTool()->IsA(UOdysseyPainterEditorRasterTransformTool::StaticClass()))
                    Cast<UOdysseyPainterEditorRasterTransformTool>(GetCurrentMainTool())->RedoTransformTransaction();
                else
                    GEditor->RedoTransaction();
            }
        )
    );

    FButtonArgs clearCanvasButtonArgs;
    clearCanvasButtonArgs.Command = FGenericCommands::Get().Redo;
    clearCanvasButtonArgs.ToolTipOverride = LOCTEXT("top-tab.clear", "Clear the whole canvas.");
    clearCanvasButtonArgs.IconOverride = FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Clear32");
    clearCanvasButtonArgs.ExtensionHook = "ClearCanvas";
    clearCanvasButtonArgs.UserInterfaceActionType = EUserInterfaceActionType::Button;
    clearCanvasButtonArgs.Action = FUIAction(
        FExecuteAction::CreateLambda(
            [this]()
            {
                TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
                if (!source)
                    return;

                source->Clear();
            }
        )
    );

    iBuilder.BeginSection("UndoRedo");
        iBuilder.AddToolBarButton(undoButtonArgs);
        iBuilder.AddToolBarButton(redoButtonArgs);
    iBuilder.EndSection();

    iBuilder.BeginSection("ClearCanvas");
        iBuilder.AddToolBarButton(clearCanvasButtonArgs);
    iBuilder.EndSection();

    UOdysseyPainterEditorTool* currentTool = GetCurrentTool();
    if (currentTool)
    {
        currentTool->ExtendToolbar(iBuilder);
    }
}

void
FOdysseyPainterEditor::OnClose()
{
    //Here is where we should clean everything prior to editor destruction
    mTabs.Empty(); //ensure all tabs are destroyed, because some need the editor on destruction

    //BE CAREFUL: OnClose can be called twice when quiting Unreal Engine
    // due to a bug in Unreal code

    SetSource(nullptr);

    for (TSharedPtr<FOdysseyPainterEditorExtension> extension : mExtensions)
        extension->Finalize();

    mGUI->Finalize();

    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    FSlateApplication::Get().UnregisterInputPreProcessor(mAnimationFlipSystem);

    delete mHUDSystem;
    mHUDSystem = nullptr;

    FOdysseyPainterEditorModule& painterEditorModule = FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
    painterEditorModule.RemoveOpenedEditor(this);
}

void
FOdysseyPainterEditor::InitHUD()
{
}

void
FOdysseyPainterEditor::InitTools()
{
    mRasterDrawingTool = AddTool<UOdysseyPainterEditorRasterDrawingTool>();
    mRasterEraserTool = AddTool<UOdysseyPainterEditorRasterEraserTool>();
    mRasterSelectionTool = AddTool<UOdysseyPainterEditorRasterSelectionTool>();
    mRasterTransformTool = AddTool<UOdysseyPainterEditorRasterTransformTool>();
    mRasterPaintBucketTool = AddTool<UOdysseyPainterEditorRasterPaintBucketTool>();
    mVectorPathDrawingTool = AddTool<UOdysseyPainterEditorVectorPathDrawingTool>();
    mVectorPathEditTool = AddTool<UOdysseyPainterEditorVectorPathEditTool>();
    mRasterPrimitiveDrawingTool = AddTool<UOdysseyPainterEditorRasterPrimitiveDrawingTool>();
    mVectorPrimitiveDrawingTool = AddTool<UOdysseyPainterEditorVectorPrimitiveDrawingTool>();
    mVectorSelectionTool = AddTool<UOdysseyPainterEditorVectorSelectionTool>();
    mVectorCutTool = AddTool<UOdysseyPainterEditorVectorCutTool>();
    mVectorScenePanTool = AddTool<UOdysseyPainterEditorVectorScenePanTool>();
    mVectorEraserTool = AddTool<UOdysseyPainterEditorVectorEraserTool>();
    mVectorPathPushTool = AddTool<UOdysseyPainterEditorVectorPathPushTool>();
    mVectorPathSmoothTool = AddTool<UOdysseyPainterEditorVectorPathSmoothTool>();
    mVectorPathStitchTool = AddTool<UOdysseyPainterEditorVectorPathStitchTool>();
    mVectorPaintBucketTool = AddTool<UOdysseyPainterEditorVectorPaintBucketTool>();
    mColorPickerTool = AddTool<UOdysseyPainterEditorColorPickerTool>();
    mVectorGridTool = AddTool<UOdysseyPainterEditorVectorGridTool>();
    mVectorTransformTool = AddTool<UOdysseyPainterEditorVectorTransformTool>();
    mVectorMatchingTool = AddTool<UOdysseyPainterEditorVectorMatchingTool>();
    mVectorChartTool = AddTool<UOdysseyPainterEditorVectorChartTool>();
    mVectorTrajectoryTool = AddTool<UOdysseyPainterEditorVectorTrajectoryTool>();
    mOutOfPegsTool = AddTool<UOdysseyPainterEditorAnimationOutOfPegsTool>();
    mRasterDrawingTool->SetBrushContexts(&mBrushContexts);
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

TArray<FOdysseyBrushContext*>&
FOdysseyPainterEditor::GetBrushContexts()
{
    return mBrushContexts;
}

FOdysseyHUD*
FOdysseyPainterEditor::HUDSystem() const
{
    return mHUDSystem;
}

const FOdysseyBrushColor&
FOdysseyPainterEditor::PaintColor() const
{
    return mPaintColor;
}

UOdysseyAnimation*
FOdysseyPainterEditor::GetAnimation() const
{
    if (!mSource || mSource->Id() != FOdysseyPainterEditorAnimationSource::StaticId() )
        return nullptr;


    TSharedPtr<FOdysseyPainterEditorAnimationSource> animationSource = StaticCastSharedPtr<FOdysseyPainterEditorAnimationSource>(mSource);
    return animationSource->GetAnimation();
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

void
FOdysseyPainterEditor::InactivateAllTools()
{
    if (!mCurrentMainTool && !mCurrentTemporaryTool)
        return;

    if (mCurrentTemporaryTool)
    {
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }

    if (mCurrentMainTool)
    {
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
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
        mOnCurrentTemporaryToolChanged.Broadcast();
    }

    if (mCurrentMainTool && mCurrentMainTool != iTool && mCurrentMainTool->IsActivated())
    {
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

    mCurrentTemporaryTool->Inactivate();
    mCurrentTemporaryTool = nullptr;
    mOnCurrentTemporaryToolChanged.Broadcast();

    if (mCurrentMainTool && mCurrentMainTool->IsActivable())
    {
        mCurrentMainTool->Activate();
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
        mCurrentTemporaryTool->Inactivate();
        mCurrentTemporaryTool = nullptr;
    }

    if (mCurrentMainTool && mCurrentMainTool->IsActivated())
    {
        mCurrentMainTool->Inactivate();
    }

    mCurrentTemporaryTool = iTool;
    mCurrentTemporaryTool->Activate();

    mOnCurrentTemporaryToolChanged.Broadcast();
    mOnCurrentToolChanged.Broadcast();
    UToolMenus::Get()->RefreshMenuWidget(mToolbarMenuName);
}

void
FOdysseyPainterEditor::SanitizeCurrentTool()
{
    InactivateAllTools();

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

TSharedPtr<FOdysseyPainterEditorRasterSelection>
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

        mRasterSelection.Reset();
        mRasterSelection = MakeShared< FOdysseyPainterEditorRasterSelection >();

        mAnimationTimelinePosition->Reset();
    }

    if (iSource)
    {
        mSource = iSource;
        mSource->OnAddEditedObjectDelegate().AddLambda([this](UObject* iObject) { AddEditedObject(iObject);});
        mSource->OnRemoveEditedObjectDelegate().AddLambda([this](UObject* iObject) { RemoveEditedObject(iObject);});
        mSource->Activate(this);

        mRasterSelection->Init(mSource->Width(), mSource->Height());

        if ( mCurrentMainTool && mCurrentMainTool->IsActivable() )
        {
            ActivateMainTool(mCurrentMainTool);
        }
        else
        {
            //select the best tool
            SanitizeCurrentTool();
        }
    }

    OnSourceChanged().Broadcast();
}

void
FOdysseyPainterEditor::PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit)
{
    mPaintColor = iColor;
    mCurrentPaletteEntryColor = nullptr;
    mCurrentPaletteSet = 0;

    //PATCH: should be automatic in the new drawing Tool, fix it asap
    if (iIsCommit)
        FOdysseyObjectEditorUtils::SetPropertyValue(GetRasterDrawingTool()->GetBrushOptions(), GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Color), iColor);
}

TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition>
FOdysseyPainterEditor::GetAnimationTimelinePosition()
{
    return mAnimationTimelinePosition;
}

UOdysseyPainterEditorTool*
FOdysseyPainterEditor::FindDefaultToolForCurrentLayer()
{
    for (UOdysseyPainterEditorTool* tool : mTools)
    {
        if ( !tool->IsActivable() )
            continue;

        return tool;
    }
    return nullptr;
}

FOdysseyPainterEditorGUI*
FOdysseyPainterEditor::GetGUI()
{
    if (!mGUI)
        mGUI = MakeShareable(new FOdysseyPainterEditorGUI(this));
    return mGUI.Get();
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.bring-forward", "Bring forward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject
                                                                           , notificationFlags );

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

    // request redraw
    //vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::SendBackward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( selectedObject )
    {
        // needed for undos
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.send-backward", "Send backward"));
        if( GUndo )
        {
           FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTransferObjects( iScene
                                                                           , selectedObject
                                                                           , notificationFlags );

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

    // request redraw
    //vectorEngine->Invalidate( 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ApplyTransformations( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // concerns only top-most objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // Backup before, for undoing
    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.apply-transformations", "Apply Transformations"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoApplyTransformations( iScene
                                                                             , objectList
                                                                             , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::MakePaintGroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorBucket*> removedBucketArray;
    std::vector<FOdysseyVectorObject*> cubicPathArray;
    std::list<FOdysseyVectorObject*> objectList;
    FOdysseyVectorGroupPaint* paintGroup;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                                  , removedBucketArray
                                                                  , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::Ungroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    FOdysseyVectorObject* selectedObject = iScene->GetCell()->GetLastSelectedObject();
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                                        , group
                                                                        , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

void
FOdysseyPainterEditor::GroupAndAddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                , FOdysseyVectorGroupPaint* iScene )
{
    // this call will also handle the creation of an Undo entry
    FOdysseyVectorGroup* group = _Group( iEditor, iScene );
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                                  , objectArray
                                                                  , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );

    return group;
}

// static
void
FOdysseyPainterEditor::Subdivide( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
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
                                                                  , addedSegmentArray
                                                                  , notificationFlags );

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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // concerns all selected objects of a branch including implicit selection
    iScene->GetCell()->GetFocusedObjectList( objectList );

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-points","Select All Points"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectVertex( iScene
                                                                     , objectList
                                                                     , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::SelectAllObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.select-all-objects", "Select All"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoSelectObject( iScene->GetLayer()
                                                                     , iScene->GetCell()
                                                                     , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetCell()->SelectAllInSelectionSpace();

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetView( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-view", "Reset view"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, iScene, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::LockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene, selectedVertexArray, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::UnlockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexLock( iScene, selectedVertexArray, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::UnalignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.unalign-point-selection","Unalign Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, selectedVertexArray, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::AlignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorVertex*> selectedVertexArray; // for undoing
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    selectedVertexArray.reserve( 50 );

    // first step prepare the array. First step is needed because we are going to snapshot
    // segment handles coordinates before they'll get aligned.
    iScene->GetCell()->GetSelectedVerticesFromFocusedObjects( selectedVertexArray );

    //------------- undo ----------------//
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.align-point-selection","Align Point Selection"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoVertexAlignment( iScene, selectedVertexArray, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
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
                                                                  , addedSegmentArray
                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::DeleteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorObject*> removedObjectArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    removedObjectArray.reserve( selectedObjectList.size() );

    iScene->GetCell()->RemoveObjects( selectedObjectList, removedObjectArray );

    iScene->GetCell()->ClearObjectSelection();

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-objects", "Delete Objects"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoRemoveObjects( iScene
                                                                      , removedObjectArray
                                                                      , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> removedTagArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-tags", "Delete Tags"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagRemove( iScene
                                                                  , removedTagArray
                                                                  , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::CommitSelectedInbetweenerTag( FOdysseyPainterEditor* iEditor
                                                   , FOdysseyVectorLayer* iLayer )
{
    std::list<FOdysseyVectorGroupPaint*> committedSceneList;
    std::list<FOdysseyVectorObject*> addedObjectList;
    std::list<FOdysseyVectorTag*> removedTagList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
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
                                                                                 , committedSceneList
                                                                                 , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        iLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        iLayer->RequestRedraw( nullptr, 0 );

        // call callbacks if any (for refreshing GUI e.g)
        iLayer->Notify( notificationFlags );
    }
}

// static
void
FOdysseyPainterEditor::AddInbetweenerTag( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = iScene->GetCell()->GetSelectedObjectList();
    std::vector<FOdysseyVectorTag*> addedTagArray;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                               , addedTagArray
                                                               , notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-grid-geometry","Paste Grid Geometry"));
        if( GUndo )
        {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerMatching( iScene
                                                                                       , selectedBreakdownList
                                                                                       , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FInbetweenerBreakdown* breakdown : selectedBreakdownList )
        {
            breakdown->GetGrid()->SetGeometry( copiedGridGeometry
                                             , eInbetweenerPointPositionType::TargetPosition
                                             , true );
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerGrid( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorGroupPaint* iScene
                                           , bool iResetTransformation
                                           , bool iResetDeformation )
{
    std::list<FOdysseyVectorTagInbetweener*> selectedInbetweenerTagList;
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
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
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Inbetweener Grid"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerReset( iScene
                                                                                , selectedInbetweenerTagList
                                                                                , iResetDeformation
                                                                                , iResetTransformation
                                                                                , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTagInbetweener* inbetweenerTag : selectedInbetweenerTagList )
        {
            FInbetweenerBreakdown* breakdown = inbetweenerTag->GetBreakdownByCellIndex( iScene->GetCell()->GetIndex() );

            if( breakdown )
            {
                if( iResetTransformation )
                {
                    breakdown->SetTargetTransform( 0.0f, 0.0f, 0.0f, 1.0f, 1.0f );
                    breakdown->UpdateMatrix();
                }

                if( iResetDeformation )
                {
                    breakdown->GetGrid()->ResetDeformation( true );
                }
            }
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetSpacingChart( FOdysseyPainterEditor* iEditor
                                        , FOdysseyVectorGroupPaint* iScene
                                        , bool iResetPositionning
                                        , bool iCurrentBreakdownOnly )
{
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;
    uint32 cellIndex = iScene->GetCell()->GetIndex();

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    if( selectedTagList.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene->GetLayer()
                                                                                     , selectedTagList
                                                                                     , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

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
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ResetInbetweenerTagSpacingChart( FOdysseyPainterEditor* iEditor
                                                      , FOdysseyVectorLayer* iLayer )
{
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
    std::list<FOdysseyVectorTag*> selectedTagList;

    iLayer->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                         , selectedTagList );

    if( selectedTagList.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-chart","Reset Spacing Chart"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iLayer
                                                                                     , selectedTagList
                                                                                     , notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        for( FOdysseyVectorTag* tag : selectedTagList )
        {
            FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

            for( FInbetweenerBreakdown* breakdown : inbetweenerTag->GetBreakdownList() )
            {
                breakdown->GetChart()->Reset( false );
            }
        }
    }

    // updated invalidated objects.
    // Updating via SharedEnv this will request a redraw as well
    iLayer->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iLayer->RequestRedraw( nullptr, 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iLayer->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::FlipHorizontal( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // concerns only the top-most selected objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-horizontal", "Flip Horizontal"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorObject::FlipObjectsHorizontal( objectList );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::FlipVertical( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    // concerns only the top-most selected objects of a branch, including the scene
    iScene->GetCell()->GetFocusedAncestorList( objectList );

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.flip-vertical", "Flip Vertical"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, objectList, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    FOdysseyVectorObject::FlipObjectsVertical( objectList );

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::ClearColoring( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene )
{
    std::vector<FOdysseyVectorBucket*> bucketArray;
    std::list<FOdysseyVectorObject*> objectList;
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( iScene, bucketArray, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::DeleteBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FOdysseyVectorObject* ownerObject = iBucket->GetOwner();
    FOdysseyVectorGroupPaint* scene = ownerObject->GetScene();
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( ownerObject->HasBaseClass( FOdysseyVectorGroupPaint::StaticClass() ) )
    {
        FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(ownerObject);

        paintGroup->RemoveBucket( iBucket );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.delete-bucket","Delete Bucket"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketRemove( scene, iBucket, notificationFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
    }

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS ); // re-colorize paint group
    scene->GetLayer()->RequestRedraw( nullptr, 0 );

    // call callbacks if any (for refreshing GUI e.g)
    scene->GetLayer()->Notify( notificationFlags );
}

void
FOdysseyPainterEditor::AlterContourWidth( FOdysseyVectorGroupPaint* iScene
                                        , double iValue
                                        , bool   iAbsolute )
{
    //std::list<FOdysseyVectorGroupPaint*> paintgroupList;
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = 0;

    // needed for valid GUndo pointer
    GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.propagate-bucket","Propagate Bucket"));
    if( GUndo )
    {
        FOdysseyVectorUndo* undo = new FOdysseyVectorUndoBucketParam( scene, iBucket, notificationFlags );

        GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
        if (source)
            source->RecordCurrentFrameUndo();
    }
    GEditor->EndTransaction();

    iBucket->SetPropagated( iPropagate );

    scene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    scene->GetLayer()->RequestRedraw( scene->GetCell(), 0 );

    // call callbacks if any (for refreshing GUI e.g)
    scene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
    uint32 cellIndex = iScene->GetCell()->GetIndex();
    std::list<FOdysseyVectorTag*> selectedTagList;

    iScene->GetLayer()->GetSelectedTagByClassType( FOdysseyVectorTagInbetweener::StaticClass()
                                                     , selectedTagList );

    if( spacingList.size() > 0 )
    {
        if( selectedTagList.size() )
        {
            // needed for valid GUndo pointer
            GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.paste-chart","Paste Spacing Chart"));
            if( GUndo )
            {
                FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerChartAlter( iScene->GetLayer()
                                                                                         , selectedTagList
                                                                                         , notificationFlags );

                GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

                TSharedPtr<FOdysseyPainterEditorSource> source = iEditor->GetSource();
                if (source)
                    source->RecordCurrentFrameUndo();
            }
            GEditor->EndTransaction();

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
        }
    }

    iScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    iScene->GetLayer()->RequestRedraw( iScene->GetCell(), 0 );

    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( selectedObject )
    {
        FOdysseyVectorObject* storeObject = GetStoreObject();

        //----- needed for undos -----//
        GEditor->BeginTransaction(LOCTEXT("PasteTransformation", "Paste Transformation"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoObjectTransform( iScene, selectedObject, notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                                                                    , pastedObjectList
                                                                                                    , notificationFlags ) );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                                  , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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

    // request redraw

    // call callbacks if any (for refreshing GUI e.g)
    iDestinationScene->GetLayer()->Notify( notificationFlags );
}

// static
void
FOdysseyPainterEditor::RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                           , FOdysseyVectorLayer* iLayer )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;
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
                                                                      , tagList
                                                                      , notificationFlags );

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
    iLayer->Notify( notificationFlags );
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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

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
                                                                            , mergedSegmentArray
                                                                            , notificationFlags );

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

    // call callbacks if any (for refreshing GUI e.g)
    iScene->GetLayer()->Notify( notificationFlags );
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
    for (UOdysseyPainterEditorTool* tool : mTools)
    {
        Collector.AddReferencedObject(tool);
    }

    for (TSharedPtr<FOdysseyPainterEditorPaletteSet> paletteSet : mPaletteSets)
    {
        UOdysseyPalette* palette = paletteSet->GetPalette();
        Collector.AddReferencedObject(palette);
    }

    Collector.AddReferencedObject(mCurrentPaletteEntryColor);
}

const TArray<TSharedPtr<FOdysseyPainterEditorPaletteSet>>&
FOdysseyPainterEditor::GetPaletteSets() const
{
    return mPaletteSets;
}

UOdysseyPaletteEntryColor*
FOdysseyPainterEditor::GetCurrentPaletteColorEntry() const
{
    return mCurrentPaletteEntryColor;
}

int
FOdysseyPainterEditor::GetCurrentPaletteSet() const
{
    return mCurrentPaletteSet;
}

void
FOdysseyPainterEditor::AddPaletteSet(TSharedPtr<FOdysseyPainterEditorPaletteSet> iPaletteSet)
{
    mPaletteSets.Add(iPaletteSet);
}

void
FOdysseyPainterEditor::RemovePaletteSet(TSharedPtr<FOdysseyPainterEditorPaletteSet> iPaletteSet)
{
    mPaletteSets.Remove(iPaletteSet);

    if (mCurrentPaletteEntryColor)
    {
        bool shouldReset = !mPaletteSets.ContainsByPredicate(
            [this](TSharedPtr<FOdysseyPainterEditorPaletteSet> iPaletteSet)
            {
                return iPaletteSet->GetPalette() == mCurrentPaletteEntryColor->GetPalette() && iPaletteSet->GetSet() == mCurrentPaletteSet;
            }
        );
        if (shouldReset)
        {
            mCurrentPaletteEntryColor = nullptr;
            mCurrentPaletteSet = 0;
        }
    }
}

/* void
FOdysseyPainterEditor::SetPaletteSet(int iIndex, const FOdysseyPainterEditorPaletteSet& iPaletteSet)
{
    if (iIndex < 0 || iIndex >= mPaletteSets.Num())
        return;

    mPaletteSets[iIndex] = iPaletteSet;

    if (mPaletteCurrentEntryColor.GetEntry() && !mPaletteSets.Contains(mPaletteCurrentEntryColor.GetPaletteSet()))
    {
        UOdysseyPalette* palette = mPaletteSets[iIndex].GetPalette();
        if (palette->ContainsEntry(mPaletteCurrentEntryColor.GetEntry()))
        {
            SetPaletteCurrentColorEntry(FOdysseyPainterEditorPaletteEntryColor(mPaletteCurrentEntryColor.GetEntry(), mPaletteSets[iIndex].GetSet()));
        }
        else
        {
            mPaletteCurrentEntryColor.Reset();
        }
    }
} */

void
FOdysseyPainterEditor::SetCurrentPaletteColorEntry(UOdysseyPaletteEntryColor* iEntry, int iSet)
{
    if (!iEntry)
    {
        mCurrentPaletteEntryColor = nullptr;
        mCurrentPaletteSet = 0;
        return;
    }

    UOdysseyPalette* palette = iEntry->GetPalette();
    if(palette && iSet >= 0 && iSet < palette->GetSets().Num())
    {
        FColor color = iEntry->GetColor(iSet);
        ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8(color.R, color.G, color.B, color.A);

        mPaintColor = ulisColor;

        //PATCH: should be automatic in the new drawing Tool, fix it asap
        FOdysseyObjectEditorUtils::SetPropertyValue(GetRasterDrawingTool()->GetBrushOptions(), GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Color), mPaintColor);
    }
    mCurrentPaletteEntryColor = iEntry;
    mCurrentPaletteSet = iSet;
}

#undef LOCTEXT_NAMESPACE
