// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditorStandaloneToolkit.h"

/* Gary
#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorAnimationSource.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditorAnimationTimelineTab.h"
#include "OdysseyPainterEditorAnimationLighttableTab.h"
#include "OdysseyPainterEditorAnimationDetailsTab.h"
#include "OdysseyPainterEditorColorSelectorTab.h"
#include "OdysseyPainterEditorFlipbookTimelineTab.h"
#include "OdysseyPainterEditorLayerStackTab.h"
#include "OdysseyPainterEditorMeshSelectorTab.h"
#include "OdysseyPainterEditorTextureDetailsTab.h"
#include "OdysseyPainterEditorToolsTab.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "OdysseyPainterEditorVectorSceneTreeViewTab.h"
#include "OdysseyPainterEditorAnimationLayout.h"
#include "OdysseyPainterEditorTextureLayout.h"
#include "OdysseyPainterEditorFlipbookLayout.h"

#include "Engine/Texture2D.h"
#include "PaperFlipbook.h"
#include "ToolMenus.h"
*/

#define LOCTEXT_NAMESPACE "ArianeEditor"

/////////////////////////////////////////////////////
// FArianeEditorStandaloneToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FArianeEditorStandaloneToolkit::~FArianeEditorStandaloneToolkit()
{
}

FArianeEditorStandaloneToolkit::FArianeEditorStandaloneToolkit(UObject* iEditedObject)
    : mEditedObject(iEditedObject)
/* Gary
   , mEditor(nullptr)
*/
{
    if (mEditedObject->IsA<AActor>())
    {
        mAppIdentifier = TEXT("ArianeEditor");
        mTitle = LOCTEXT("3D-painting-editor.name", "3D Painting Editor");
        mWorldCentricTabPrefix = LOCTEXT( "3D-painting-editor.world-centric-tab-prefix", "3D Painting " ).ToString();
    }
}

void
FArianeEditorStandaloneToolkit::Open()
{
/* Gary
    mEditor = MakeShared<FArianeEditor>(SharedThis(this));
    mEditor->Initialize();
    mEditor->InitTabs();

    TArray<UObject*> editedObjects = mEditor->GetAdditionalEditedObjects(); //Editor can add some side edited objects
    editedObjects.Add(mEditedObject);


    const FName MenuName = GetToolMenuToolbarName();
    //Must be called before InitAssetEditor
    if (!UToolMenus::Get()->IsMenuRegistered(MenuName))
    {
        UToolMenu* ToolBar = UToolMenus::Get()->RegisterMenu(MenuName, "AssetEditor.DefaultToolBar", EMultiBoxType::ToolBar);
        mEditor->ExtendAssetEditorToolbar( ToolBar );
    }
    else
    {
        mEditor->ExtendAssetEditorToolbar( UToolMenus::Get()->ExtendMenu(MenuName) );
    }

    TSharedPtr<FTabManager::FLayout> layout;
    if (mEditedObject->IsA<UOdysseyAnimation>())
    {
        layout = FArianeEditorAnimationLayout::Create();
    }
    else if (mEditedObject->IsA<UTexture2D>())
    {
        layout = FArianeEditorTextureLayout::Create();
    }
    else if (mEditedObject->IsA<UPaperFlipbook>())
    {
        layout = FArianeEditorFlipbookLayout::Create();
    }
*/

    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, mAppIdentifier, layout.ToSharedRef(), true, true, editedObjects);
/* Gary
    //Add Odyssey Specific section to the main menu to add entries at the right place easier
    UToolMenu* fileMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".File")));
    fileMenu->FindOrAddSection("OdysseyFile");

    UToolMenu* editMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Edit")));
    editMenu->FindOrAddSection("OdysseyEdit");

    UToolMenu* assetMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Asset")));
    assetMenu->FindOrAddSection("OdysseyAsset");

    UToolMenu* windowMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Window")));
    windowMenu->FindOrAddSection("OdysseyWindow");

    UToolMenu* toolsMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Tools")));
    toolsMenu->FindOrAddSection("OdysseyTools");

    UToolMenu* helpMenu = UToolMenus::Get()->ExtendMenu(*(GetToolMenuName().ToString() + FString(".Help")));
    helpMenu->FindOrAddSection("OdysseyHelp");

    TSharedRef<FExtender> menuExtender = MakeShared<FExtender>();
    mEditor->ExtendMenu( menuExtender );
    AddMenuExtender(menuExtender);

    mEditor->BindShortcuts( this );
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FArianeEditorStandaloneToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FArianeEditorStandaloneToolkit::OnRemoveEditedObject);

    mEditor->OnRegenerateToolbarAndMenus().BindLambda(
        [this]()
        {
            RegenerateMenusAndToolbars();
        }
    );
    RegenerateMenusAndToolbars();

    mEditor->SetEditedObject(mEditedObject);
*/
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

FLinearColor
FArianeEditorStandaloneToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

void
FArianeEditorStandaloneToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
/* Gary
    FAssetEditorToolkit::InitToolMenuContext(MenuContext);
    mEditor->InitToolMenuContext(MenuContext);
*/
}

void
FArianeEditorStandaloneToolkit::SaveAssetAs_Execute()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddLambda(
        [](UObject* iObject)
        {
            FArianeEditorModule* arianeEditorModule = &FModuleManager::GetModuleChecked<FArianeEditorModule>("ArianeEditor");
            arianeEditorModule->OpenStandaloneEditorForAsset(iObject);
        }
    );

    FAssetEditorToolkit::SaveAssetAs_Execute();

    AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);
}

bool
FArianeEditorStandaloneToolkit::OnRequestClose()
{
/* Gary
    return mEditor->OnCloseRequested();
*/
    return true;
}

void
FArianeEditorStandaloneToolkit::OnClose()
{
/* Gary
    mEditor->OnClose();
*/
}

void
FArianeEditorStandaloneToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
/* Gary
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    mEditor->RegisterTabSpawners(iTabManager);
*/
}

void
FArianeEditorStandaloneToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
/* Gary
    FAssetEditorToolkit::UnregisterTabSpawners(iTabManager);
    mEditor->UnregisterTabSpawners(iTabManager);
*/
}

bool
FArianeEditorStandaloneToolkit::CanReimport() const
{
    return false;
}

bool
FArianeEditorStandaloneToolkit::CanReimport(UObject* EditingObject) const
{
    return false;
}

FText
FArianeEditorStandaloneToolkit::GetToolkitName() const
{
    return GetLabelForObject(GetEditingObjects()[0]);
}

FName
FArianeEditorStandaloneToolkit::GetToolkitFName() const
{
    return mAppIdentifier;
}

FText
FArianeEditorStandaloneToolkit::GetBaseToolkitName() const
{
    return mTitle;
}

FString
FArianeEditorStandaloneToolkit::GetWorldCentricTabPrefix() const
{
    return mWorldCentricTabPrefix;
}

FText
FArianeEditorStandaloneToolkit::GetToolkitToolTipText() const
{
    return GetToolTipTextForObject(GetEditingObjects()[0]);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FArianeEditorStandaloneToolkit::OnAddEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

void
FArianeEditorStandaloneToolkit::OnRemoveEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

#undef LOCTEXT_NAMESPACE
