// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorGUI.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageVector.h"
#include "OdysseyAnimationCellImageVector.h"
#include "OdysseyEditorTab.h"
#include "OdysseyLayerStack.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorAnimationLightTableTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorCommands.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureLayerStack.h"
#include "OdysseyVectorGroupPaint.h"

#include "SOdysseyTabletAPISwitcher.h"

#include "Framework/Docking/LayoutExtender.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Toolkits/AssetEditorModeUILayer.h"

#include "RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorGUI::~FOdysseyPainterEditorGUI()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll( this );
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll(this);
    UOdysseyLayer::OnMediaChanged().RemoveAll( this );
}

FOdysseyPainterEditorGUI::FOdysseyPainterEditorGUI(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{
    // Get sure the binding is set up everytime we add or remove a layer in the layer stack.
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyPainterEditorGUI::OnCurrentLayerChanged );

    UOdysseyAnimation::OnCurrentFrameChanged().AddRaw( this, &FOdysseyPainterEditorGUI::OnCurrentFrameChanged );

    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &FOdysseyPainterEditorGUI::OnVectorSceneNotify );
    // bind refresh function to delegates on existing vector scenes when the source changes. Needed to refresh necessary widgets.
    mEditor->OnSourceChanged().AddRaw( this, &FOdysseyPainterEditorGUI::OnSourceChanged );

    UOdysseyLayer::OnMediaChanged().AddRaw( this, &FOdysseyPainterEditorGUI::OnMediaChanged );
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tabs

void
FOdysseyPainterEditorGUI::Initialize()
{
    CreateTabs();
}

void
FOdysseyPainterEditorGUI::Finalize()
{
}

void
FOdysseyPainterEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{
    Extender.ExtendLayout(FTabId(TEXT("PlacementBrowser")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorToolsTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSceneOutliner")), ELayoutExtensionPosition::Below, FTabManager::FTab(FOdysseyPainterEditorColorSelectorTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::ClosedTab));
    Extender.ExtendLayout(FTabId(TEXT("Sequencer")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyPainterEditorAnimationTimelineTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyPainterEditorGUI::CreateTabs()
{
    TSharedRef<FOdysseyPainterEditorMeshSelectorTab> meshSelectorTab = MakeShared<FOdysseyPainterEditorMeshSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorViewportTab> viewportTab = MakeShared<FOdysseyPainterEditorViewportTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorColorSelectorTab> colorSelectorTab = MakeShared<FOdysseyPainterEditorColorSelectorTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorToolsTab> toolsTab = MakeShared<FOdysseyPainterEditorToolsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = MakeShared<FOdysseyPainterEditorVectorSceneTreeViewTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyPainterEditorLayerStackTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorTextureDetailsTab> textureDetailsTab = MakeShared<FOdysseyPainterEditorTextureDetailsTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorFlipbookTimelineTab> flipbookTimelineTab = MakeShared<FOdysseyPainterEditorFlipbookTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationTimelineTab> animationTimelineTab = MakeShared<FOdysseyPainterEditorAnimationTimelineTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationLightTableTab> animationLightTableTab = MakeShared<FOdysseyPainterEditorAnimationLightTableTab>(mEditor);
    TSharedRef<FOdysseyPainterEditorAnimationDetailsTab> animationDetailsTab = MakeShared<FOdysseyPainterEditorAnimationDetailsTab>(mEditor);

    //Used for the viewport drawing editor to know which tab to open by default

    flipbookTimelineTab->ShouldOpenByDefault(true);
    colorSelectorTab->ShouldOpenByDefault(true);
    toolsTab->ShouldOpenByDefault(true);
    layerStackTab->ShouldOpenByDefault(true);
    animationTimelineTab->ShouldOpenByDefault(true);

    mEditor->AddTab(toolsTab);
    mEditor->AddTab(meshSelectorTab);
    mEditor->AddTab(viewportTab);
    mEditor->AddTab(colorSelectorTab);
    mEditor->AddTab(vectorSceneTreeViewTab);
    mEditor->AddTab(layerStackTab);
    mEditor->AddTab(textureDetailsTab);
    mEditor->AddTab(flipbookTimelineTab);
    mEditor->AddTab(animationTimelineTab);
    mEditor->AddTab(animationLightTableTab);
    mEditor->AddTab(animationDetailsTab);
}

void
FOdysseyPainterEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorCommands.SwitchTabletAPI, SwitchTabletAPI )

    //Need to rethink the commands and shortcuts to put them in the right place and not in GUI
    MAP_ACTION(painterEditorCommands.ClearCurrentLayer, ClearCurrentLayer)
    MAP_ACTION(painterEditorCommands.SelectAll, SelectAll)
    MAP_ACTION(painterEditorCommands.ClearCurrentSelection, ClearCurrentSelection)
    MAP_ACTION(painterEditorCommands.InvertSelection, InvertSelection)

    MAP_ACTION(painterEditorCommands.ToggleEraserButton, ToggleEraserButton)

    #undef MAP_ACTION
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Layout

void
FOdysseyPainterEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FLayout> layout = iBuilder.GetLayout();

    TSharedRef<FTabManager::FArea> mainArea = iBuilder.CreateArea("MainArea");
    mainArea->SetOrientation(Orient_Horizontal);
    layout->AddArea(mainArea);

    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.CreateSplitter("MainVerticalSplitter");
    mainVerticalSplitter->SetOrientation(Orient_Vertical);
    mainVerticalSplitter->SetSizeCoefficient(1.f);

    mainArea->Split
    (
        mainVerticalSplitter
    );

    TSharedRef<FTabManager::FSplitter> mainHorizontalSplitter = iBuilder.CreateSplitter("MainHorizontalSplitter");
    mainHorizontalSplitter->SetOrientation(Orient_Horizontal);
    mainHorizontalSplitter->SetSizeCoefficient(1.f);

    mainVerticalSplitter->Split
    (
        mainHorizontalSplitter
    );

    TSharedRef<FTabManager::FSplitter> leftSplitter = iBuilder.CreateSplitter("LeftSplitter");
    leftSplitter->SetOrientation(Orient_Vertical);
    leftSplitter->SetSizeCoefficient(0.15f);

    TSharedRef<FTabManager::FSplitter> centerSplitter = iBuilder.CreateSplitter("CenterSplitter");
    centerSplitter->SetOrientation(Orient_Vertical);
    centerSplitter->SetSizeCoefficient(0.7f);

    TSharedRef<FTabManager::FSplitter> rightSplitter = iBuilder.CreateSplitter("RightSplitter");
    rightSplitter->SetOrientation(Orient_Vertical);
    rightSplitter->SetSizeCoefficient(0.15f);

    mainHorizontalSplitter->Split
    (
        leftSplitter
    );

    mainHorizontalSplitter->Split
    (
        centerSplitter
    );

    mainHorizontalSplitter->Split
    (
        rightSplitter
    );

    CreateLeftSection(iBuilder);
    CreateCenterSection(iBuilder);
    CreateRightSection(iBuilder);
    CreateBottomSection(iBuilder);
}

void
FOdysseyPainterEditorGUI::CreateLeftSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> leftSplitter = iBuilder.GetSplitter("LeftSplitter");

    TSharedRef<FTabManager::FStack> currentToolStack = iBuilder.CreateStack("CurrentToolStack");
    currentToolStack->SetHideTabWell(false);
    currentToolStack->SetSizeCoefficient(0.33f);
    currentToolStack->AddTab(FOdysseyPainterEditorVectorSceneTreeViewTab::StaticId(),ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> toolsStack = iBuilder.CreateStack("ToolsStack");
    toolsStack->SetHideTabWell(false);
    toolsStack->SetSizeCoefficient(0.33f);
    toolsStack->AddTab(FOdysseyPainterEditorToolsTab::StaticId(), ETabState::OpenedTab);

    leftSplitter->Split
    (
        currentToolStack
    );

    leftSplitter->Split
    (
        toolsStack
    );
}

void
FOdysseyPainterEditorGUI::CreateRightSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> rightSplitter = iBuilder.GetSplitter("RightSplitter");

    TSharedRef<FTabManager::FStack> colorSelectorStack = iBuilder.CreateStack("ColorSelectorStack");
    colorSelectorStack->SetHideTabWell(false);
    colorSelectorStack->SetSizeCoefficient(0.3f);
    colorSelectorStack->AddTab(FOdysseyPainterEditorColorSelectorTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> textureLayerStackStack = iBuilder.CreateStack("TextureLayerStackStack");
    textureLayerStackStack->SetHideTabWell(false);
    textureLayerStackStack->SetSizeCoefficient(0.35f);
    textureLayerStackStack->AddTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> textureDetailsStack = iBuilder.CreateStack("TextureDetailsStack");
    textureDetailsStack->SetHideTabWell(false);
    textureDetailsStack->SetSizeCoefficient(0.15f);
    textureDetailsStack->AddTab(FOdysseyPainterEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

    rightSplitter->Split
    (
        colorSelectorStack
    );

    rightSplitter->Split
    (
        textureLayerStackStack
    );

    rightSplitter->Split
    (
        textureDetailsStack
    );
}

void
FOdysseyPainterEditorGUI::CreateCenterSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> centerSplitter = iBuilder.GetSplitter("CenterSplitter");

    TSharedRef<FTabManager::FStack> viewportStack = iBuilder.CreateStack("viewportStack");
    viewportStack->SetHideTabWell(false);
    viewportStack->SetSizeCoefficient(0.975f);
    viewportStack->AddTab(FOdysseyPainterEditorViewportTab::StaticId(), ETabState::OpenedTab);

    centerSplitter->Split
    (
        viewportStack
    );
}

void
FOdysseyPainterEditorGUI::CreateBottomSection(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.GetSplitter("MainVerticalSplitter");
    TSharedRef<FTabManager::FStack> flipbookTimelineStack = iBuilder.CreateStack("FlipbookTimelineStack");
    flipbookTimelineStack->SetHideTabWell(false);
    flipbookTimelineStack->SetSizeCoefficient(0.2f);
    flipbookTimelineStack->AddTab(FOdysseyPainterEditorFlipbookTimelineTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> animationTimelineStack = iBuilder.CreateStack("AnimationTimelineStack");
    animationTimelineStack->SetHideTabWell(false);
    animationTimelineStack->SetSizeCoefficient(0.2f);
    animationTimelineStack->AddTab(FOdysseyPainterEditorAnimationTimelineTab::StaticId(), ETabState::OpenedTab);

    mainVerticalSplitter->Split
    (
        animationTimelineStack
    );

    mainVerticalSplitter->Split
    (
        flipbookTimelineStack
    );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts

void
FOdysseyPainterEditorGUI::SwitchTabletAPI()
{
    SOdysseyTabletAPISwitcher::Open();
}

void FOdysseyPainterEditorGUI::ClearCurrentLayer()
{
    if( mEditor && mEditor->GetSource() )
        mEditor->GetSource()->Clear();
}

void FOdysseyPainterEditorGUI::ToggleEraserButton()
{
    if( mEditor )
    {
        if( mEditor->GetCurrentTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()) )
        {
            Cast< UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetCurrentTool())->BlendParameters.bEraserMode = !Cast< UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetCurrentTool())->BlendParameters.bEraserMode;
        }
    }
}

void
FOdysseyPainterEditorGUI::SelectAll()
{
    TArray<FVector2D> polyPoints;
    polyPoints.Add(FVector2D(0, 0));
    polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), 0));
    polyPoints.Add(FVector2D(mEditor->RasterSelection()->GetBlock()->Width(), mEditor->RasterSelection()->GetBlock()->Height()));
    polyPoints.Add(FVector2D(0, mEditor->RasterSelection()->GetBlock()->Height()));

    mEditor->RasterSelection()->Add(polyPoints);
}

void
FOdysseyPainterEditorGUI::ClearCurrentSelection()
{
    mEditor->RasterSelection()->Clear();
}

void FOdysseyPainterEditorGUI::InvertSelection()
{
    mEditor->RasterSelection()->Invert();
}

void
FOdysseyPainterEditorGUI::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyTextureLayerImageVector* currentTextureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->CurrentLayer.Get());
    UOdysseyAnimationLayerImageVector* currentAnimationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->CurrentLayer.Get());

    if( currentTextureVectorLayer || currentAnimationVectorLayer )
    {
        OnSourceChanged();
    }
    else
    {
        ParseVectorNotifications( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
    }
}

void
FOdysseyPainterEditorGUI::OnCurrentFrameChanged( UOdysseyAnimation* iAnimation )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            ParseVectorNotifications( vectorScene, notificationFlags );

            vectorScene->GetLayer()->RequestRedraw( vectorScene->GetCell(), 0 );
        }
        else
        {
            ParseVectorNotifications( nullptr, notificationFlags );
        }
    }
    else
    {
        ParseVectorNotifications( nullptr, notificationFlags );
    }
}

void
FOdysseyPainterEditorGUI::OnSourceChanged()
{
    if( mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            OnVectorSceneNotify( vectorScene, FOdysseyVectorEngine::NOTIFY_ALL );
        }
        else
        {
            OnVectorSceneNotify( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
        }
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyPainterEditorGUI::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags )
{
    UOdysseyLayerStack* layerStack = mEditor->LayerStack();

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageVector* currentTextureVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
        UOdysseyAnimationLayerImageVector* currentAnimationVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

        if(  currentAnimationVectorLayer )
        {
            int frame = currentAnimationVectorLayer->GetAnimation()->CurrentFrame;
            UOdysseyAnimationCell* cell = currentAnimationVectorLayer->GetCellAtFrame(frame);
            if (cell && cell->IsA<UOdysseyAnimationCellImageVector>())
            {
                UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);
                // Note: iScene is ignored. We update the widget according to the current scene if any.
                FOdysseyVectorGroupPaint* vectorScene = cellVector->GetVectorCell()->GetScene();
                ParseVectorNotifications( vectorScene, iSignalFlags );
                return;
            }
        }
        else if (currentTextureVectorLayer)
        {
            FOdysseyVectorGroupPaint* currentScene = currentTextureVectorLayer->GetVectorCell()->GetScene();
            ParseVectorNotifications( currentScene, iSignalFlags );

            return;
        }
    }
    // for some reason when Unreal loads, the layerstack is NULL. But the medias exist. So in that case we use
    // the media provider.
    else
    {
        if( mEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
        {
            TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

            if( mediaVectors.Num() )
            {
                FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

                ParseVectorNotifications( vectorScene, iSignalFlags );

                return;
            }
        }
    }

    ParseVectorNotifications( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
}

void
FOdysseyPainterEditorGUI::ParseVectorNotifications( FOdysseyVectorGroupPaint* iScene
                                                    , uint64 iSignalFlags )
{
    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mEditor->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        if( vectorSceneTreeViewTab != nullptr )
        {
            vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( iScene );
        }
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mEditor->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        if( vectorSceneTreeViewTab != nullptr )
        {
            vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( iScene );
        }
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
/*
        TSharedPtr<FOdysseyPainterEditorAnimationTimelineTab> timelineTab = mEditor->FindTab<FOdysseyPainterEditorAnimationTimelineTab>();
        TSharedPtr<SWidgetSwitcher> widgetSwitcher = StaticCastSharedPtr<SWidgetSwitcher>(timelineTab.Get()->Widget());
        TSharedPtr<SOdysseyAnimationLayerStack> layerStack = StaticCastSharedPtr<SOdysseyAnimationLayerStack>(widgetSwitcher.Get()->GetWidget(0));
        TSharedPtr<SOdysseyLayerStackTreeView> treeView = layerStack.Get()->GetTreeView();
        TSharedPtr<SOdysseyAnimationTimelineTreeView> timelineTreeView = layerStack.Get()->GetTimelineTreeView();
        TArray<UOdysseyLayer*> selectedItemArray;

        selectedItemArray = treeView.Get()->GetItems();

        for( UOdysseyLayer* layer : selectedItemArray )
        {
            //treeView.Get()->GenerateNewWidget( layer );
            TSharedPtr<ITableRow> headerTableRow = treeView.Get()->WidgetFromItem ( layer );
            TSharedPtr<ITableRow> timelineTableRow = timelineTreeView.Get()->WidgetFromItem ( layer );

            if( headerTableRow.IsValid() )
            {
                TSharedRef<SWidget> rowWidget = headerTableRow.Get()->AsWidget();

                if( rowWidget.Get().GetType() == "SOdysseyAnimationLayerImageVectorRow" )
                {
                    // Update the left part of the timeline
                    TSharedRef<SOdysseyAnimationLayerImageVectorRow> vectorRowWidget = StaticCastSharedRef<SOdysseyAnimationLayerImageVectorRow>(rowWidget);
                    vectorRowWidget->GetInbetweeningHeader()->Update();
                }
            }

            if( timelineTableRow.IsValid() )
            {
                TSharedRef<SWidget> rowWidget = timelineTableRow.Get()->AsWidget();

                if( rowWidget.Get().GetType() == "SOdysseyAnimationLayerImageVectorTimeline" )
                {
                       TSharedPtr<SOdysseyAnimationLayerImageVectorTimeline> vectorTimelineWidget = StaticCastSharedRef<SOdysseyAnimationLayerImageVectorTimeline>(rowWidget);
                    vectorTimelineWidget.Get()->GetInbetweeningListView().Get()->Update();
                }
            }
        }
*/
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_HUD )
    {
        if( iScene )
        {
            iScene->GetCell()->ResetHUD();
        }
    }
}

void
FOdysseyPainterEditorGUI::OnMediaChanged()
{
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(mEditor->LayerStack()->CurrentLayer.Get());
    uint64 returnFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                       | FOdysseyPainterEditor::UI_UPDATE_HUD;

#ifdef unused
    std::list<FOdysseyVectorTagInbetweener*> inbetweenerTagList;

    if( currentVectorLayer )
    {

        // check the validity of inbetweener tags and prepare a list for processing
        for( FOdysseyVectorTag* tag : currentVectorLayer->GetLayer()->GetSharedTagList() )
        {
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);
                int32 targetCellIndex = inbetweenerTag->GetDrawingBuffer().back().GetCellIndex();

                if( currentVectorLayer->GetCellByIndex( targetCellIndex ) == nullptr )
                {
                    inbetweenerTagList.push_back( inbetweenerTag );
                }
            }
        }
/*
        //------------- undo ----------------//
        GEditor->BeginTransaction(LOCTEXT("vector-scene.transaction.reset-breakdown-layout","Reset Breakdown Layout"));
        if( GUndo )
        {
            FOdysseyVectorUndo* undo = new FOdysseyVectorUndoTagInbetweenerBreakdownAlter( currentVectorLayer->GetLayer()
                                                                                         , inbetweenerTagList
                                                                                         , returnFlags );

            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
        //---------- end of undo ------------//
*/
        // proceed
        for( FOdysseyVectorTagInbetweener* inbetweenerTag : inbetweenerTagList )
        {
            inbetweenerTag->ResetLayout( false );
            inbetweenerTag->GetBreakdownList().back()->SetTargetDrawingIndex( 1 );
        }
    }
#endif

    ParseVectorNotifications( nullptr, FOdysseyPainterEditor::UI_UPDATE_TIMELINE );
}

#undef LOCTEXT_NAMESPACE
