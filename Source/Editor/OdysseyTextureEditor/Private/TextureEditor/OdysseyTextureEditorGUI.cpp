// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorGUI"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	FOdysseyVectorScene::OnSignalDelegate().RemoveAll(this);
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor),
	mEditor( iEditor )
{
    // Get sure the binding is set up everytime we add or remove a layer in the layer stack.
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyTextureEditorGUI::OnCurrentLayerChanged );
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorScene::OnSignalDelegate().AddRaw( this, &FOdysseyTextureEditorGUI::OnVectorSceneSignal );
}

void
FOdysseyTextureEditorGUI::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        OnVectorSceneSignal( vectorScene, FOdysseyVectorScene::SIGNALL_ALL );
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyTextureEditorGUI::CreateTabs()
{
	FOdysseyPainterEditorGUI::CreateTabs();

	//ADD NEW TABS
	ODYSSEY_ADD_TAB(mLayerStackTab, FOdysseyTextureEditorLayerStackTab, mEditor);
	ODYSSEY_ADD_TAB(mTextureDetailsTab, FOdysseyTextureEditorTextureDetailsTab, mEditor);
}

TSharedRef<FTabManager::FSplitter>
FOdysseyTextureEditorGUI::CreateRightSection()
{
	return FOdysseyPainterEditorGUI::CreateRightSection()
		// LayerStack + Notes
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mLayerStackTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.35f)
		)
		->Split
		(
			FTabManager::NewStack()
			->AddTab(mTextureDetailsTab->ID(), ETabState::OpenedTab)
			->SetHideTabWell(false)
			->SetSizeCoefficient(0.15f)
		);
}

void
FOdysseyTextureEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyPainterEditorGUI::BindShortcuts(iToolkit);

    //---

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(textureEditorCommands.ResetView, ResetView )
    MAP_ACTION(textureEditorCommands.GroupPaint, GroupPaint )
    MAP_ACTION(textureEditorCommands.Group, Group )
    MAP_ACTION(textureEditorCommands.Ungroup, Ungroup )
    MAP_ACTION(textureEditorCommands.BringForward, BringForward )
    MAP_ACTION(textureEditorCommands.SendBackward, SendBackward )
    MAP_ACTION(textureEditorCommands.RemoveSelectedObjects, RemoveSelectedObjects )

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Menu and Toolbar

void
FOdysseyTextureEditorGUI::ExtendMenuAbout( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyPainterEditorGUI::ExtendMenuAbout( iOwner, iMenuName );

    //---

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
            FOdysseyTextureEditorCommands::Get().RemoveSelectedObjects
            ,LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
            ,LOCTEXT("RemoveSelectedObjects","RemoveSelectedObjects")
            ,FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
            ,NAME_None);
    }
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FName
FOdysseyTextureEditorGUI::GetLayoutName()
{
	return "OdysseyTextureEditor_Layout";
}

TSharedPtr<FOdysseyTextureEditorLayerStackTab>&
FOdysseyTextureEditorGUI::GetLayerStackTab()
{
	return mLayerStackTab;
}

TSharedPtr<FOdysseyTextureEditorTextureDetailsTab>&
FOdysseyTextureEditorGUI::GetTextureDetailsTab()
{
	return mTextureDetailsTab;
}

void
FOdysseyTextureEditorGUI::OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags )
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer && ( currentVectorLayer->GetScene() == iScene ) )
    {
        if( iSignalFlags & FOdysseyVectorScene::SCENE_REDRAW )
        {
            currentVectorLayer->RenderImageChanged( false );
        }

        if( ( iSignalFlags & FOdysseyVectorScene::OBJECT_TRANSFORMED )
         || ( iSignalFlags & FOdysseyVectorScene::OBJECT_SELECTED    )
         || ( iSignalFlags & FOdysseyVectorScene::OBJECT_MODIFIED    ) )
        {
            vectorObjectTab.Get()->Update( iScene );
        }
    }
}

void
FOdysseyTextureEditorGUI::RemoveSelectedObjects()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::RemoveSelectedObjects( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::BringForward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::BringForward( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::SendBackward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::SendBackward( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::Ungroup()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::Ungroup( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::ResetView()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::ResetView( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::Group()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::Group( vectorEngine, vectorScene, currentVectorLayer );
    }
}

void
FOdysseyTextureEditorGUI::GroupPaint()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();

        FOdysseyPainterEditorGUI::GroupPaint( vectorEngine, vectorScene, currentVectorLayer );
    }
}

#undef LOCTEXT_NAMESPACE
