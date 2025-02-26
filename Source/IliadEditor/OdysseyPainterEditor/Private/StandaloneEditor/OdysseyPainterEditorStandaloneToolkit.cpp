// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorStandaloneToolkit.h"

#include "OdysseyAnimation.h"
#include "OdysseyPainterEditorModule.h"
#include "OdysseyPainterEditor.h"

#include "Engine/Texture2D.h"
#include "PaperFlipbook.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorStandaloneToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorStandaloneToolkit::~FOdysseyPainterEditorStandaloneToolkit()
{
}

FOdysseyPainterEditorStandaloneToolkit::FOdysseyPainterEditorStandaloneToolkit(UObject* iEditedObject)
    : mEditedObject(iEditedObject)
    , mEditor(nullptr)
{
    if (mEditedObject->IsA<UOdysseyAnimation>())
    {
        mAppIdentifier = TEXT("OdysseyAnimationEditor");
        mTitle = LOCTEXT("2D-animation-painting-editor.name", "2D Animation Painting Editor");
        mLayoutName = "OdysseyAnimationEditor_Layout";
        mWorldCentricTabPrefix = LOCTEXT( "2D-animation-painting-editor.world-centric-tab-prefix", "2D Animation " ).ToString();
    }
    else if (mEditedObject->IsA<UTexture2D>())
    {
        mAppIdentifier = TEXT("OdysseyTextureEditor");
        mTitle = LOCTEXT("texture-painting-editor.name", "Texture Painting Editor");
        mLayoutName = "OdysseyTextureEditor_Layout";
        mWorldCentricTabPrefix = LOCTEXT( "texture-painting-editor.world-centric-tab-prefix", "Texture " ).ToString();
    }
    else if (mEditedObject->IsA<UPaperFlipbook>())
    {
        mAppIdentifier = TEXT("OdysseyFlipbookEditor");
        mTitle = LOCTEXT("flipbook-painting-editor.name", "Flipbook Painting Editor");
        mLayoutName = "OdysseyFlipbookEditor_Layout";
        mWorldCentricTabPrefix = LOCTEXT( "flipbook-painting-editor.world-centric-tab-prefix", "Flipbook " ).ToString();
    }
}

void
FOdysseyPainterEditorStandaloneToolkit::Open()
{
    mEditor = MakeShared<FOdysseyPainterEditor>(SharedThis(this));
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

    FAssetEditorToolkit::InitAssetEditor( EToolkitMode::Standalone, NULL, mAppIdentifier, mEditor->CreateLayout(), true, true, editedObjects);

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
    mEditor->OnAddEditedObjectDelegate().AddRaw(this, &FOdysseyPainterEditorStandaloneToolkit::OnAddEditedObject);
    mEditor->OnRemoveEditedObjectDelegate().AddRaw(this, &FOdysseyPainterEditorStandaloneToolkit::OnRemoveEditedObject);

    mEditor->OnRegenerateToolbarAndMenus().BindLambda(
        [this]()
        {
            RegenerateMenusAndToolbars();
        }
    );
    RegenerateMenusAndToolbars();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface

FLinearColor
FOdysseyPainterEditorStandaloneToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

void
FOdysseyPainterEditorStandaloneToolkit::InitToolMenuContext(FToolMenuContext& MenuContext)
{
    FAssetEditorToolkit::InitToolMenuContext(MenuContext);
    mEditor->InitToolMenuContext(MenuContext);
}

void
FOdysseyPainterEditorStandaloneToolkit::SaveAssetAs_Execute()
{
    UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
    FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddLambda(
        [](UObject* iObject)
        {
            FOdysseyPainterEditorModule* painterEditorModule = &FModuleManager::GetModuleChecked<FOdysseyPainterEditorModule>("OdysseyPainterEditor");
            painterEditorModule->OpenStandaloneEditorForAsset(iObject);
        }
    );

    FAssetEditorToolkit::SaveAssetAs_Execute();

    AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);
}

bool
FOdysseyPainterEditorStandaloneToolkit::OnRequestClose()
{
    return mEditor->OnCloseRequested();
}

void
FOdysseyPainterEditorStandaloneToolkit::OnClose()
{
    //Here is where we should clean everything prior to editor destruction
    mEditor->OnClose();
}

void
FOdysseyPainterEditorStandaloneToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
    mEditor->RegisterTabSpawners(iTabManager);
}

void
FOdysseyPainterEditorStandaloneToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FAssetEditorToolkit::UnregisterTabSpawners(iTabManager);
    mEditor->UnregisterTabSpawners(iTabManager);
}

bool
FOdysseyPainterEditorStandaloneToolkit::CanReimport() const
{
    return false;
}

bool
FOdysseyPainterEditorStandaloneToolkit::CanReimport(UObject* EditingObject) const
{
    return false;
}

FText
FOdysseyPainterEditorStandaloneToolkit::GetToolkitName() const
{
    return GetLabelForObject(GetEditingObjects()[0]);
}

FName
FOdysseyPainterEditorStandaloneToolkit::GetToolkitFName() const
{
    return mAppIdentifier;
}

FText
FOdysseyPainterEditorStandaloneToolkit::GetBaseToolkitName() const
{
    return mTitle;
}

FString
FOdysseyPainterEditorStandaloneToolkit::GetWorldCentricTabPrefix() const
{
    return mWorldCentricTabPrefix;
}

FText
FOdysseyPainterEditorStandaloneToolkit::GetToolkitToolTipText() const
{
    return GetToolTipTextForObject(GetEditingObjects()[0]);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyPainterEditorStandaloneToolkit::OnAddEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

void
FOdysseyPainterEditorStandaloneToolkit::OnRemoveEditedObject(UObject* iObject)
{
    AddEditingObject(iObject);
}

#undef LOCTEXT_NAMESPACE
