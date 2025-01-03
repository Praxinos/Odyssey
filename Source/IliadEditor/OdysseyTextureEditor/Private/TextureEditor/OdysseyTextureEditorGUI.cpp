// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "TextureEditor/OdysseyTextureEditorGUI.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "OdysseyTextureEditorLayerStackTab.h"
#include "OdysseyTextureEditorTextureDetailsTab.h"
#include "TextureEditor/OdysseyTextureEditorExtension.h"
#include "Framework/Docking/LayoutExtender.h"
#include "PainterEditor/OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "TextureEditor/OdysseyTextureEditorCommands.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
// Vector engine
#include "OdysseyVectorGroupPaint.h"

#define LOCTEXT_NAMESPACE "TextureEditor"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorGUI::~FOdysseyTextureEditorGUI()
{
    UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    FOdysseyVectorEngine::OnNotifyDelegate().RemoveAll(this);
}

FOdysseyTextureEditorGUI::FOdysseyTextureEditorGUI(FOdysseyTextureEditorExtension* iExtension) :
    mExtension( iExtension )
{
    // Get sure the binding is set up everytime we add or remove a layer in the layer stack.
    UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw( this, &FOdysseyTextureEditorGUI::OnCurrentLayerChanged );
    // bind refresh function to delegates on existing vector scenes at load. Needed to refresh necessary widgets.
    FOdysseyVectorEngine::OnNotifyDelegate().AddRaw( this, &FOdysseyTextureEditorGUI::OnVectorSceneNotify );
    // bind refresh function to delegates on existing vector scenes when the source changes. Needed to refresh necessary widgets.
    mExtension->GetEditor()->OnSourceChanged().AddRaw( this, &FOdysseyTextureEditorGUI::OnSourceChanged );
}

void
FOdysseyTextureEditorGUI::OnCurrentLayerChanged( UOdysseyLayerStack* iLayerStack )
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(iLayerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorGroupPaint* vectorScene = vectorEngine->GetScene();

        OnVectorSceneNotify( vectorScene, FOdysseyVectorEngine::NOTIFY_ALL );
    }
    else
    {
        TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

        vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( nullptr );
        vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( nullptr );
    }
}


void
FOdysseyTextureEditorGUI::OnSourceChanged()
{
    if( mExtension->GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>() )
    {
        // It would be better if this is done in OnMouseDown()
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = mExtension->GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() > 0 )
        {
            FOdysseyVectorGroupPaint* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            OnVectorSceneNotify( vectorScene, FOdysseyVectorEngine::NOTIFY_ALL );
        }
        else
        {
            OnVectorSceneNotify( nullptr, FOdysseyVectorEngine::NOTIFY_ALL );
        }
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
FOdysseyTextureEditorGUI::ExtendMenu( TSharedRef<FExtender> iExtender )
{
}

void
FOdysseyTextureEditorGUI::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyTextureEditorCommands& textureEditorCommands = FOdysseyTextureEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyTextureEditorGUI::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyTextureEditorGUI::OnVectorSceneNotify( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags )
{
    TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
    if (!source)
        return;

    TSharedPtr<FOdysseyPainterEditorVectorSceneTreeViewTab> vectorSceneTreeViewTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorVectorSceneTreeViewTab>();

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(source->GetLayerStack());

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentVectorLayer )
        {
            FOdysseyVectorGroupPaint* currentScene = currentVectorLayer->GetEngine()->GetScene();

            if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW )
            {
                vectorSceneTreeViewTab.Get()->UpdateSceneTreeView( ( iScene == nullptr ) ? currentScene
                                                                                         : iScene );
            }

            if( iSignalFlags & FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS )
            {
                vectorSceneTreeViewTab.Get()->UpdateObjectPropertiesPanel( ( iScene == nullptr ) ? currentScene
                                                                                                 : iScene );
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE
