// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"
#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"
#include "AnimationEditor/OdysseyAnimationEditorAnimationDetailsTab.h"
#include "Framework/Docking/LayoutExtender.h"
#include "PainterEditor/OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyAnimation.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/OdysseyAnimationCellsContainer.h"
#include "LayerStack/Cells/CellImageVector/OdysseyAnimationCellImageVector.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"

#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/LayerStack/SOdysseyAnimationLayerStack.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeader.h"
#include "Widgets/LayerStack/SOdysseyAnimationTimelineInbetweeningHeaderRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorRow.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimeline.h"
#include "Widgets/LayerStack/Layers/LayerImageVector/SOdysseyAnimationLayerImageVectorTimelineInbetweening.h"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorGUI::~FOdysseyAnimationEditorGUI()
{
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll( this );
	UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll( this );
	FOdysseyVectorEngine::OnSignalDelegate().RemoveAll( this );
}

FOdysseyAnimationEditorGUI::FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension)
	: mExtension(iExtension)
{
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnCurrentLayerChanged );

    UOdysseyAnimation::OnCurrentFrameChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnCurrentFrameChanged );
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnSignalDelegate().AddRaw( this, &FOdysseyAnimationEditorGUI::OnVectorSceneSignal );
    // bind refresh function to delegates on existing vector scenes when the source changes. Needed to refresh necessary widgets.
    mExtension->GetEditor()->OnSourceChanged().AddRaw( this, &FOdysseyAnimationEditorGUI::OnSourceChanged );
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

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
        ParseVectorSignal( nullptr, FOdysseyVectorEngine::SIGNAL_ALL );
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
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnVectorSceneSignal( vectorScene, FOdysseyVectorEngine::SIGNAL_ALL );
        }
        else
        {
            ParseVectorSignal( nullptr, FOdysseyVectorEngine::SIGNAL_ALL );
        }
    }
}

void
FOdysseyAnimationEditorGUI::OnCurrentFrameChanged( UOdysseyAnimation* iAnimation )
{
    if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();

            ParseVectorSignal( vectorScene, FOdysseyVectorEngine::SIGNAL_ALL );
        }
        else
        {
            ParseVectorSignal( nullptr, FOdysseyVectorEngine::SIGNAL_ALL );
        }
    }
    else
    {
        ParseVectorSignal( nullptr, FOdysseyVectorEngine::SIGNAL_ALL );
    }
}

void
FOdysseyAnimationEditorGUI::ParseVectorSignal( FOdysseyVectorGroupPaint* iScene
                                             , uint64 iSignalFlags )
{
    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( iScene );
            
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS )
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( iScene );
    }

    if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_TIMELINE )
    {
        TSharedPtr<FOdysseyAnimationEditorTimelineTab> timelineTab = mExtension->GetEditor()->FindTab<FOdysseyAnimationEditorTimelineTab>();
        TSharedPtr<SWidgetSwitcher> widgetSwitcher = StaticCastSharedPtr<SWidgetSwitcher>(timelineTab.Get()->Widget());
        TSharedPtr<SOdysseyAnimationLayerStack> layerStack = StaticCastSharedPtr<SOdysseyAnimationLayerStack>(widgetSwitcher.Get()->GetWidget(0));
        TSharedPtr<SOdysseyLayerStackTreeView> treeView = layerStack.Get()->GetTreeView();
        TArray<UOdysseyLayer*> selectedItemArray;

        selectedItemArray = treeView.Get()->GetItems();

        for( UOdysseyLayer* layer : selectedItemArray )
        {
            //treeView.Get()->GenerateNewWidget( layer );
            TSharedPtr<ITableRow> tableRow = treeView.Get()->WidgetFromItem ( layer );

            if( tableRow.IsValid() )
            {
                TSharedRef<SWidget> rowWidget = tableRow.Get()->AsWidget();

                if( rowWidget.Get().GetType() == "SOdysseyAnimationLayerImageVectorRow" )
                {
                    TSharedRef<SOdysseyAnimationLayerImageVectorRow> vectorRowWidget = StaticCastSharedRef<SOdysseyAnimationLayerImageVectorRow>(rowWidget);
                    TSharedPtr<SOdysseyAnimationLayerImageVectorTimeline> vectorTimelineWidget = StaticCastSharedPtr<SOdysseyAnimationLayerImageVectorTimeline>(vectorRowWidget.Get().GetTimelineWidget());

                    // Update the left part of the timeline
                    vectorRowWidget.Get().GetInbetweeningHeader().Get()->Update();

                    vectorTimelineWidget.Get()->GetInbetweeningListView().Get()->Update();
                }
            }
        }
    }
}

void
FOdysseyAnimationEditorGUI::OnVectorSceneSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags )
{
    UOdysseyAnimationLayerStack* layerStack = mExtension->LayerStack();

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyAnimationLayerImageVector* currentVectorLayer = Cast<UOdysseyAnimationLayerImageVector>(layerStack->CurrentLayer.Get());
        int frame = mExtension->Animation()->CurrentFrame;
        FOdysseyAnimationCellImageVector* cell = static_cast<FOdysseyAnimationCellImageVector*>(currentVectorLayer->GetCellsContainer()->GetCellAtFrame(frame).Get());

        if( cell )
        {
            FOdysseyVectorEngine* vectorEngine = cell->GetEngine();
            // Note: iScene is ignored. We update the widget according to the current scene.
            FOdysseyVectorGroupPaint* vectorScene = vectorEngine->GetScene();

            ParseVectorSignal( vectorScene, iSignalFlags );
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

                ParseVectorSignal( vectorScene, FOdysseyVectorEngine::SIGNAL_ALL );
            }
        }
    }
}
