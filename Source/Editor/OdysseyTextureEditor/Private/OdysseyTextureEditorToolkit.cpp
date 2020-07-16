// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureEditorToolkit.h"

#include "OdysseyTextureEditorController.h"
#include "OdysseyTextureEditorData.h"
#include "OdysseyTextureEditorGUI.h"

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
	mController->Init(ToolkitCommands);
	mGUI->Init(mData, mController);

	FOdysseyPainterEditorToolkit::InitPainterEditorToolkit(iMode, iInitToolkitHost, iAppIdentifier, iTexture);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyTextureEditorToolkit::SaveAsset_Execute()
{
	mData->SyncTextureAndInvalidate();

    FAssetEditorToolkit::SaveAsset_Execute();
}

void
FOdysseyTextureEditorToolkit::SaveAssetAs_Execute()
{
    /*
    CopyBlockDataIntoUTexture( mDisplaySurface->Block(), mTexture );
    //::ul3::FMakeContext::CopyBlockInto( mDisplaySurface->Block()->GetBlock(), mTextureContentsBackup->GetBlock() );
    InvalidateTextureFromData( mDisplaySurface->Block(), mTexture );
    // Invalidate all
    mDisplaySurface->Invalidate();

    FAssetEditorToolkit::SaveAssetAs_Execute();

    CopyBlockDataIntoUTexture( mTextureContentsBackup, mTexture );

    InvalidateTextureFromData( mTextureContentsBackup, mTexture );
    InvalidateSurfaceFromData( mTextureContentsBackup, mDisplaySurface );
    */
}

bool
FOdysseyTextureEditorToolkit::OnRequestClose()
{
	mData->SyncTextureAndInvalidate();
	mData->ApplyPropertiesBackup();

	//TODO: Move in the right place
    mData->LayerStack()->mDrawingUndo->Clear();
    return true;
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