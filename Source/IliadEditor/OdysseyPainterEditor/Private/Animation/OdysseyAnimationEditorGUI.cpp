// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationEditorGUI.h"

#include "OdysseyAnimationEditorExtension.h"
#include "OdysseyAnimationEditorLightTableTab.h"
#include "OdysseyAnimationEditorTimelineTab.h"
#include "OdysseyAnimationEditorAnimationDetailsTab.h"
#include "Framework/Docking/LayoutExtender.h"
#include "PainterEditor/OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorTagInbetweener.h"
#include "InbetweenerTag/InbetweenerBreakdown.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerBreakdownAlter.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Animation/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/Animation/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/Animation/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/Animation/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "Widgets/Animation/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/Animation/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"

#include "Widgets/Animation/LayerStack/SOdysseyAnimationTimelineTreeView.h"

#define LOCTEXT_NAMESPACE "AnimationEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorGUI::~FOdysseyAnimationEditorGUI()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll( this );
    UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll( this );
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll( this );
    UOdysseyLayer::OnMediaChanged().RemoveAll( this );
}

FOdysseyAnimationEditorGUI::FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension)
    : mExtension(iExtension)
{
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnCurrentLayerChanged );

    UOdysseyAnimation::OnCurrentFrameChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnCurrentFrameChanged );
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &FOdysseyAnimationEditorGUI::OnVectorSceneNotify );
    // bind refresh function to delegates on existing vector scenes when the source changes. Needed to refresh necessary widgets.
    mExtension->GetEditor()->OnSourceChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnSourceChanged );

    UOdysseyLayer::OnMediaChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnMediaChanged );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditorGUI::Initialize()
{
    CreateTabs();
}

void
FOdysseyAnimationEditorGUI::Finalize()
{

}

void
FOdysseyAnimationEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.GetSplitter("MainVerticalSplitter");
    TSharedRef<FTabManager::FStack> animationTimelineStack = iBuilder.CreateStack("AnimationTimelineStack");
    animationTimelineStack->SetHideTabWell(false);
    animationTimelineStack->SetSizeCoefficient(0.2f);
    animationTimelineStack->AddTab(FOdysseyAnimationEditorTimelineTab::StaticId(), ETabState::OpenedTab);

    mainVerticalSplitter->Split
    (
        animationTimelineStack
    );
}

void
FOdysseyAnimationEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{
    Extender.ExtendLayout(FTabId(TEXT("Sequencer")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyAnimationEditorTimelineTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyAnimationEditorGUI::CreateTabs()
{
    //ADD NEW TABS
    TSharedRef<FOdysseyAnimationEditorTimelineTab> layerStackTab = MakeShared<FOdysseyAnimationEditorTimelineTab>(mExtension);
    TSharedRef<FOdysseyAnimationEditorLightTableTab> lightTableTab = MakeShared<FOdysseyAnimationEditorLightTableTab>(mExtension);
    TSharedRef<FOdysseyAnimationEditorAnimationDetailsTab> animationDetailsTab = MakeShared<FOdysseyAnimationEditorAnimationDetailsTab>(mExtension);

    layerStackTab->ShouldOpenByDefault(true);

    mExtension->GetEditor()->AddTab(layerStackTab);
    mExtension->GetEditor()->AddTab(lightTableTab);
    mExtension->GetEditor()->AddTab(animationDetailsTab);
}

/* TSharedRef<FTabManager::FSplitter>
FOdysseyAnimationEditorGUI::CreateBottomSection()
{
    return FOdysseyPainterEditorGUI::CreateBottomSection()
        // Timeline
        ->Split
        (
            FTabManager::NewStack()
            ->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.35f)
        );
} */

void
FOdysseyAnimationEditorGUI::OnMediaChanged()
{
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(mExtension->GetEditor()->LayerStack()->CurrentLayer.Get());
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

            TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
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

void
FOdysseyAnimationEditorGUI::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(iLayerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        OnSourceChanged();
    }
    else
    {
        ParseVectorNotifications( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
    }
}

void
FOdysseyAnimationEditorGUI::OnSourceChanged()
{
    if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            OnVectorSceneNotify( vectorScene, FOdysseyVectorEngine::NOTIFY_ALL );
        }
        else
        {
            ParseVectorNotifications( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
        }
    }
}

void
FOdysseyAnimationEditorGUI::OnCurrentFrameChanged( UOdysseyAnimation* iAnimation )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                             | FOdysseyPainterEditor::UI_UPDATE_HUD;

    if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

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
FOdysseyAnimationEditorGUI::ParseVectorNotifications( FOdysseyVectorGroupPaint* iScene
                                                    , uint64 iSignalFlags )
{
    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        if( vectorSceneTreeViewTab != nullptr )
        {
            vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( iScene );
        }
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        if( vectorSceneTreeViewTab != nullptr )
        {
            vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( iScene );
        }
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
/*
        TSharedPtr<FOdysseyAnimationEditorTimelineTab> timelineTab = mExtension->GetEditor()->FindTab<FOdysseyAnimationEditorTimelineTab>();
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
FOdysseyAnimationEditorGUI::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iNotificationFlags )
{
    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentVectorLayer )
        {
            int frame = mExtension->Animation()->CurrentFrame;
            UOdysseyAnimationCell* cell = currentVectorLayer->GetCellAtFrame(frame);
            if (cell && cell->IsA<UOdysseyAnimationCellImageVector>())
            {
                UOdysseyAnimationCellImageVector* cellVector = Cast<UOdysseyAnimationCellImageVector>(cell);
                // Note: iScene is ignored. We update the widget according to the current scene if any.
                FOdysseyVectorGroupPaint* vectorScene = cellVector->GetVectorCell()->GetScene();
                ParseVectorNotifications( vectorScene, iNotificationFlags );
                return;
            }
        }
    }
    // for some reason when Unreal loads, the layerstack is NULL. But the medias exist. So in that case we use
    // the media provider.
    else
    {
        if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
        {
            TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

            if( mediaVectors.Num() )
            {
                FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

                ParseVectorNotifications( vectorScene, iNotificationFlags );

                return;
            }
        }
    }

    ParseVectorNotifications( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
}

#undef LOCTEXT_NAMESPACE
