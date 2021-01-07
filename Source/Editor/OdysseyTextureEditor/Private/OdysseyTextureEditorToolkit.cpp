// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorToolkit.h"

#include "OdysseyTextureEditorController.h"
#include "OdysseyTextureEditorData.h"
#include "OdysseyTextureEditorGUI.h"

#include "IOdysseyTextureEditorModule.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorToolkit::~FOdysseyTextureEditorToolkit()
{
}

FOdysseyTextureEditorToolkit::FOdysseyTextureEditorToolkit()
{
}

void
FOdysseyTextureEditorToolkit::Init(const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, UTexture2D* iTexture)
{
	mData = MakeShareable(new FOdysseyTextureEditorData(iTexture));
	mGUI = MakeShareable(new FOdysseyTextureEditorGUI());
	mController = MakeShareable(new FOdysseyTextureEditorController(mData, mGUI));

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init(ToolkitCommands);

	TArray<UObject*> objectsToEdit;
	objectsToEdit.Add(iTexture);

	FOdysseyPainterEditorToolkit::InitPainterEditorToolkit(iMode, iInitToolkitHost, iAppIdentifier, objectsToEdit);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTextureEditorToolkit::SaveAsset_Execute()
{
	/* mData->PaintEngine()->Flush();
	mData->SyncTextureAndInvalidate();
	mData->ApplyPropertiesBackup(); */

    FAssetEditorToolkit::SaveAsset_Execute();
	
	// mData->PrepareTextureProperties();
}

void
FOdysseyTextureEditorToolkit::SaveAssetAs_Execute()
{
	/* mData->PaintEngine()->Flush();
	mData->SyncTextureAndInvalidate();
	mData->ApplyPropertiesBackup(); */

	//PATCH: Intercept Open request to open the asset in iliad instead of the default editor
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddRaw(this, &FOdysseyTextureEditorToolkit::OpenAsset);

	FAssetEditorToolkit::SaveAssetAs_Execute();

	// mData->PrepareTextureProperties();

	AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);
}

void
FOdysseyTextureEditorToolkit::OpenAsset(UObject* iObject)
{
	UTexture2D* texture = Cast<UTexture2D>(iObject);
	IOdysseyTextureEditorModule* odysseyTextureEditorModule = &FModuleManager::GetModuleChecked<IOdysseyTextureEditorModule>("OdysseyTextureEditor");
	odysseyTextureEditorModule->CreateOdysseyTextureEditor(EToolkitMode::Standalone, NULL, texture);
}

bool
FOdysseyTextureEditorToolkit::OnRequestClose()
{
	mData->PaintEngine()->Flush();
	mData->SyncTextureAndInvalidate();
	mData->ApplyPropertiesBackup();

	//TODO: Move in the right place
    mData->LayerStack()->mDrawingUndo->Clear();
    return true;
}

FText
FOdysseyTextureEditorToolkit::GetToolkitName() const
{
	return GetLabelForObject(mData->Texture());
}

FText
FOdysseyTextureEditorToolkit::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(mData->Texture());
}

FText
FOdysseyTextureEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Texture Editor" );
}

FName
FOdysseyTextureEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyTextureEditor" );
}

FLinearColor
FOdysseyTextureEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

FString
FOdysseyTextureEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Texture" ).ToString();
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyTextureEditorToolkit::GetLayout() const
{
    return mGUI->GetLayout();
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyTextureEditorToolkit::GetMenuExtenders() const
{
	//mController->GetMenuExtenders();
	const FOdysseyTextureEditorController* controller = mController.Get();
	return controller->GetMenuExtenders();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
void
FOdysseyTextureEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FOdysseyPainterEditorToolkit::RegisterTabSpawners(iTabManager);

    WorkspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyTextureEditor", "Odyssey Texture Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, WorkspaceMenuCategoryRef);
}

void
FOdysseyTextureEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FOdysseyPainterEditorToolkit::UnregisterTabSpawners(iTabManager);

	mGUI->UnregisterTabSpawners(iTabManager);
}

#undef LOCTEXT_NAMESPACE