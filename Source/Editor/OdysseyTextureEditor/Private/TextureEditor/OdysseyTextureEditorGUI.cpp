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
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditor* iEditor) :
	FOdysseyPainterEditorGUI(iEditor),
	mEditor( iEditor )
{
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
FOdysseyTextureEditorGUI::RemoveSelectedObjects()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("RemoveSelectedObjects", "Remove Selected Objects"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::RemoveSelectedObjects( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );

        vectorObjectTab.Get()->Update( vectorScene );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::BringForward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("BringForward", "Bring Forward"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::BringForward( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::SendBackward()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("SendBackward", "Send Backward"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::SendBackward( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::Ungroup()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ResetView", "Reset View"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::Ungroup( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );

        vectorObjectTab.Get()->Update( vectorScene );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::ResetView()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("ResetView", "Reset View"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::ResetView( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::Group()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("Group", "Group"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::Group( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );

        vectorObjectTab.Get()->Update( vectorScene );
    }

    GEditor->EndTransaction();
}

void
FOdysseyTextureEditorGUI::GroupPaint()
{
    TSharedPtr<FOdysseyPainterEditorSelectedVectorObjectTab>& vectorObjectTab = mEditor->GetGUI()->GetSelectedVectorObjectTab();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(static_cast<FOdysseyTextureEditor*>(mEditor)->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    // needed for undos
    GEditor->BeginTransaction(LOCTEXT("GroupPaint", "Group Paint"));

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = currentVectorLayer->GetScene();
        FOdysseyVectorUndo* undo = nullptr;

        FOdysseyPainterEditorGUI::GroupPaint( vectorEngine, vectorScene, &undo );

        if( undo && GUndo )
        {
            GUndo->StoreUndo( currentVectorLayer, TUniquePtr<FOdysseyVectorUndo>(undo) );
            // All the delegates for undos are added here for easier maintainability
            undo->mRefreshDelegate.AddRaw( vectorObjectTab.Get(), &FOdysseyPainterEditorSelectedVectorObjectTab::OnRefresh );
            undo->mRefreshDelegate.AddUObject( currentVectorLayer, &UOdysseyTextureLayerImageVector::OnRefresh );
        }

        currentVectorLayer->RenderImageChanged( false );

        vectorObjectTab.Get()->Update( vectorScene );
    }

    GEditor->EndTransaction();
}

#undef LOCTEXT_NAMESPACE
