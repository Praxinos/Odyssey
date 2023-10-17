// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
#include "AnimationEditor/OdysseyAnimationEditorLightTableTab.h"
#include "AnimationEditor/OdysseyAnimationEditorTimelineTab.h"
#include "Framework/Docking/LayoutExtender.h"
#include "PainterEditor/OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyMediaVector.h"
#include "OdysseyVectorEngine.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditorGUI::~FOdysseyAnimationEditorGUI()
{
	FOdysseyVectorEngine::OnSignalDelegate().RemoveAll( this );
}

FOdysseyAnimationEditorGUI::FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension)
	: mExtension(iExtension)
{
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnSignalDelegate().AddRaw( this, &FOdysseyAnimationEditorGUI::OnVectorSceneSignal );
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

	layerStackTab->ShouldOpenByDefault(true);

	mExtension->GetEditor()->AddTab(layerStackTab);
	mExtension->GetEditor()->AddTab(lightTableTab);
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

TSharedPtr<FOdysseyAnimationEditorTimelineTab>&
FOdysseyAnimationEditorGUI::GetTimelineTab()
{
	return mTimelineTab;
}

TSharedPtr<FOdysseyAnimationEditorLightTableTab>&
FOdysseyAnimationEditorGUI::GetLightTableTab()
{
	return mLightTableTab;
}

void
FOdysseyAnimationEditorGUI::OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags )
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source)
        return;

    // layerStack might be NULL when closing the program
    if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        if( mediaVectors.Num() )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();

            if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY )
            {
                vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( iScene );
            }

            if( ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED )
             || ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED    )
             || ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED    ) )
            {
                vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( iScene );
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE
