// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFlipbookEditorToolkit.h"

#include "OdysseyFlipbookEditorController.h"
#include "OdysseyFlipbookEditorData.h"
#include "OdysseyFlipbookEditorGUI.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorToolkit::~FOdysseyFlipbookEditorToolkit()
{
}

FOdysseyFlipbookEditorToolkit::FOdysseyFlipbookEditorToolkit()
{
}

void
FOdysseyFlipbookEditorToolkit::Init(const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, UPaperFlipbook* iFlipbook)
{
	mData = MakeShareable(new FOdysseyFlipbookEditorData(iFlipbook));
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI());
	mController = MakeShareable(new FOdysseyFlipbookEditorController(mData, mGUI));

    mController->OnSpriteCreated().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnSpriteCreated);
    mController->OnTextureCreated().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnTextureCreated);
    mController->OnKeyframeRemoved().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnKeyframeRemoved);

	mData->Init();
	mController->Init(ToolkitCommands);
	mGUI->Init(mData, mController);

	TArray<UObject*> objectsToEdit;
	objectsToEdit.Add(iFlipbook);

	FOdysseyFlipbookUtils flipbookUtils(mData->Flipbook());
	for (int32 index = 0; index < iFlipbook->GetNumKeyFrames(); ++index)
	{
		UTexture2D* texture = flipbookUtils.GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        objectsToEdit.Add(texture);
	}

	FOdysseyPainterEditorToolkit::InitPainterEditorToolkit(iMode, iInitToolkitHost, iAppIdentifier, objectsToEdit);
}

void
FOdysseyFlipbookEditorToolkit::OnSpriteCreated(UPaperSprite* iSprite)
{
	//AddEditingObject(iSprite);
}

void
FOdysseyFlipbookEditorToolkit::OnTextureCreated(UTexture2D* iTexture)
{
	AddEditingObject(iTexture);
}

void
FOdysseyFlipbookEditorToolkit::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
{
	if (!iKeyframe.Sprite)
		return;

	RemoveEditingObject(iKeyframe.Sprite);

	UTexture2D* texture = iKeyframe.Sprite->GetSourceTexture();
	if (!texture)
		return;

	RemoveEditingObject(texture);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------- FAssetEditorToolkit override

void
FOdysseyFlipbookEditorToolkit::SaveAsset_Execute()
{
	mData->SyncTextureWithSurfaceBlock();

	//Small trick
	//We want to save all sprites, but we don't want to be considered the actual sprite editor
	//So we set ourselves as editing all sprites just before saving and cancel this just after saving
	FOdysseyFlipbookUtils flipbookUtils(mData->Flipbook());
	for (int i = 0; i < mData->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = flipbookUtils.GetKeyframeSprite(i);
		if (!sprite)
			continue;

		AddEditingObject(sprite);
	}

    FAssetEditorToolkit::SaveAsset_Execute();

	for (int i = 0; i < mData->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = flipbookUtils.GetKeyframeSprite(i);
		if (!sprite)
			continue;

		RemoveEditingObject(sprite);
	}

    // mData->Flipbook()->MarkPackageDirty(); //PATCH:
}

void
FOdysseyFlipbookEditorToolkit::SaveAssetAs_Execute()
{
    /*
    CopyBlockDataIntoUFlipbook( mDisplaySurface->Block(), mFlipbook );
    //::ul3::FMakeContext::CopyBlockInto( mDisplaySurface->Block()->GetBlock(), mFlipbookContentsBackup->GetBlock() );
    InvalidateFlipbookFromData( mDisplaySurface->Block(), mFlipbook );
    // Invalidate all
    mDisplaySurface->Invalidate();

    FAssetEditorToolkit::SaveAssetAs_Execute();

    CopyBlockDataIntoUFlipbook( mFlipbookContentsBackup, mFlipbook );

    InvalidateFlipbookFromData( mFlipbookContentsBackup, mFlipbook );
    InvalidateSurfaceFromData( mFlipbookContentsBackup, mDisplaySurface );
    */
}

bool
FOdysseyFlipbookEditorToolkit::OnRequestClose()
{
	mData->ApplyPropertiesBackup();
    mData->SyncTextureWithSurfaceBlock();

	//TODO: Move in the right place
	//TODO: Do it on every texture
	if (mData->LayerStack())
	{
		mData->LayerStack()->mDrawingUndo->Clear();
	}
    return true;
}

FText
FOdysseyFlipbookEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "AppLabel", "Odyssey Flipbook Editor" );
}

FText
FOdysseyFlipbookEditorToolkit::GetToolkitName() const
{
	return GetLabelForObject(mData->Flipbook());
}


FText
FOdysseyFlipbookEditorToolkit::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(mData->Flipbook());
}

FName
FOdysseyFlipbookEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyFlipbookEditor" );
}

FLinearColor
FOdysseyFlipbookEditorToolkit::GetWorldCentricTabColorScale() const
{
    return FLinearColor( 0.3f, 0.2f, 0.5f, 0.5f );
}

FString
FOdysseyFlipbookEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "WorldCentricTabPrefix", "Flipbook" ).ToString();
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyFlipbookEditorToolkit::GetLayout() const
{
    return mGUI->GetLayout();
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyFlipbookEditorToolkit::GetMenuExtenders() const
{
	const FOdysseyFlipbookEditorController* controller = mController.Get();
	return controller->GetMenuExtenders();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
void
FOdysseyFlipbookEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FOdysseyPainterEditorToolkit::RegisterTabSpawners(iTabManager);

    WorkspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyFlipbookEditor", "Odyssey Flipbook Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, WorkspaceMenuCategoryRef);
}

void
FOdysseyFlipbookEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    FOdysseyPainterEditorToolkit::UnregisterTabSpawners(iTabManager);

	mGUI->UnregisterTabSpawners(iTabManager);
}

#undef LOCTEXT_NAMESPACE