// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureEditorLayerStackTab.h"
#include "OdysseyTextureEditorTextureDetailsTab.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "PainterEditor/OdysseyPainterEditorSelectedVectorObjectTab.h"
#include "Framework/Docking/LayoutExtender.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	FOdysseyVectorEngine::OnSignalDelegate().RemoveAll(this);
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditorExtension* iExtension) :
	mExtension( iExtension )
{
    // Get sure the binding is set up everytime we add or remove a layer in the layer stack.
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyTextureEditorGUI::OnCurrentLayerChanged );
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnSignalDelegate().AddRaw( this, &FOdysseyTextureEditorGUI::OnVectorSceneSignal );
}

void
FOdysseyTextureEditorGUI::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        //FOdysseyVectorEngine::ClearHUD();

        OnVectorSceneSignal( vectorScene, FOdysseyVectorEngine::SIGNAL_ALL );
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{   
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyTextureEditorLayerStackTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyTextureEditorGUI::Init()
{
    CreateTabs();
}

void
FOdysseyTextureEditorGUI::CreateTabs()
{
	//ADD NEW TABS
    TSharedRef<FOdysseyTextureEditorLayerStackTab> layerStackTab = MakeShared<FOdysseyTextureEditorLayerStackTab>(mExtension);
    TSharedRef<FOdysseyTextureEditorTextureDetailsTab> textureDetailsTab = MakeShared<FOdysseyTextureEditorTextureDetailsTab>(mExtension);

    layerStackTab->ShouldOpenByDefault(true);

    mExtension->GetEditor()->AddTab(layerStackTab);
    mExtension->GetEditor()->AddTab(textureDetailsTab);
}

void
FOdysseyTextureEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{	
	TSharedRef<FTabManager::FSplitter> rightSplitter = iBuilder.GetSplitter("RightSplitter");
    
    TSharedRef<FTabManager::FStack> textureLayerStackStack = iBuilder.CreateStack("TextureLayerStackStack");
	textureLayerStackStack->SetHideTabWell(false);
	textureLayerStackStack->SetSizeCoefficient(0.35f);
	textureLayerStackStack->AddTab(FOdysseyTextureEditorLayerStackTab::StaticId(), ETabState::OpenedTab);

    TSharedRef<FTabManager::FStack> textureDetailsStack = iBuilder.CreateStack("TextureDetailsStack");
	textureDetailsStack->SetHideTabWell(false);
	textureDetailsStack->SetSizeCoefficient(0.15f);
	textureDetailsStack->AddTab(FOdysseyTextureEditorLayerStackTab::StaticId(), ETabState::OpenedTab);
	
    rightSplitter->Split
	(
		textureLayerStackStack
	);

    rightSplitter->Split
	(
		textureDetailsStack
	);
}

/* TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateRightSection()
{
	return FOdysseyPainterEditorGUI::CreateRightSection()
		// LayerStack + Notes
		->Split
		(
			FTabManager::NewStack()
			->AddTab(FOdysseyTextureEditorLayerStackTab::StaticId(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.35f)
		)
		->Split
		(
			FTabManager::NewStack()
			->AddTab(FOdysseyTextureEditorTextureDetailsTab::StaticId(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.15f)
		);
} */

void
FOdysseyTextureEditorGUI::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    ExtendMenuAbout(iOwner, iMenuName);
}

void
FOdysseyTextureEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(textureEditorCommands.ResetView, ResetView )
    MAP_ACTION(textureEditorCommands.GroupPaint, GroupPaint )
    MAP_ACTION(textureEditorCommands.Group, Group )
    MAP_ACTION(textureEditorCommands.Ungroup, Ungroup )
    MAP_ACTION(textureEditorCommands.BringForward, BringForward )
    MAP_ACTION(textureEditorCommands.SendBackward, SendBackward )
    MAP_ACTION(textureEditorCommands.DeleteSelection, DeleteSelection )
    MAP_ACTION(textureEditorCommands.FlipHorizontal, FlipHorizontal )
    MAP_ACTION(textureEditorCommands.FlipVertical, FlipVertical )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar
void
FOdysseyTextureEditorGUI::ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName )
{
    UToolMenu* menu = UToolMenus::Get()->FindMenu(*(iMenuName.ToString() + FString(".Iliad")));

    //Adding entries in our menu
    FToolMenuSection& aboutSection = menu->AddSection("About ILIAD", LOCTEXT("OdysseyPainter", "ILIAD"));
    {
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().ResetView
            , LOCTEXT("ResetView", "ResetView")
            , LOCTEXT("ResetView", "ResetView")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().GroupPaint
            , LOCTEXT("GroupPaint", "GroupPaint")
            , LOCTEXT("GroupPaint", "GroupPaint")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().Group
            , LOCTEXT("Group", "Group")
            , LOCTEXT("Group", "Group")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().Ungroup
            , LOCTEXT("Ungroup", "Ungroup")
            , LOCTEXT("Ungroup", "Ungroup")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().BringForward
            , LOCTEXT("BringForward", "Bring forward")
            , LOCTEXT("BringForward", "Bring forward")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().SendBackward
            , LOCTEXT("SendBackward", "Send backward")
            , LOCTEXT("SendBackward", "Send backward")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().DeleteSelection
            , LOCTEXT("DeleteSelection","DeleteSelection")
            , LOCTEXT("DeleteSelection","DeleteSelection")
            , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().FlipHorizontal
            , LOCTEXT("FlipHorizontal","FlipHorizontal")
            , LOCTEXT("FlipHorizontal","FlipHorizontal")
            , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
            , NAME_None );
        aboutSection.AddMenuEntry(
            FOdysseyTextureEditorCommands::Get().FlipVertical
            , LOCTEXT("FlipVertical","FlipVertical")
            , LOCTEXT("FlipVertical","FlipVertical")
            , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
            , NAME_None );
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyTextureEditorGUI::OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags )
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source)
        return;

    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab> vectorObjectTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorSelectedVectorObjectTab>();
    TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();
    
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(source->GetLayerStack());

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentVectorLayer && ( currentVectorLayer->GetEngine()->GetScene() == iScene ) )
        {
            if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW )
            {
                currentVectorLayer->RenderImageChanged( false );
            }

            if( iSignalFlags & FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY )
            {
                vectorSceneTreeViewTab.Get()->Update( iScene );
            }

            if( ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_TRANSFORMED )
             || ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_SELECTED    )
             || ( iSignalFlags & FOdysseyVectorEngine::SIGNAL_OBJECT_MODIFIED    ) )
            {
                vectorObjectTab.Get()->Update( iScene );
            }
        }
    }
}

void
FOdysseyTextureEditorGUI::DeleteSelection()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->DeleteObjectSelection( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::BringForward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->BringForward( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::SendBackward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->SendBackward( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::Ungroup()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->Ungroup( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::ResetView()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->ResetView( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::Group()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->Group( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::GroupPaint()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->GroupPaint( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::FlipHorizontal()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->FlipHorizontal( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::FlipVertical()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->FlipVertical( vectorEngine, vectorScene );
    }
}

void
FOdysseyTextureEditorGUI::StitchVertices()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(mExtension->GetEditor()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        mExtension->GetEditor()->StitchVertices( vectorEngine, vectorScene );
    }
}

#undef LOCTEXT_NAMESPACE
