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
	mData->FlipbookWrapper()->OnSpriteTextureChanged().Remove(mOnSpriteTextureChangedHandle);
}

FOdysseyFlipbookEditorToolkit::FOdysseyFlipbookEditorToolkit()
{
}

void
FOdysseyFlipbookEditorToolkit::Init(const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, UPaperFlipbook* iFlipbook)
{
	TSharedPtr<FOdysseyFlipbookWrapper> flipbookWrapper = MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook));
	mOnSpriteTextureChangedHandle = flipbookWrapper->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorToolkit::OnSpriteTextureChanged);
	mData = MakeShareable(new FOdysseyFlipbookEditorData(flipbookWrapper, SharedThis(this)));
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI());
	mController = MakeShareable(new FOdysseyFlipbookEditorController(mData, mGUI));

    mController->OnSpriteCreated().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnSpriteCreated);
    mController->OnTextureCreated().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnTextureCreated);
    mController->OnKeyframeRemoved().BindRaw(this, &FOdysseyFlipbookEditorToolkit::OnKeyframeRemoved);

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init(ToolkitCommands);

	TArray<UObject*> objectsToEdit;
	objectsToEdit.Add(iFlipbook);

	for (int32 index = 0; index < iFlipbook->GetNumKeyFrames(); ++index)
	{
		UTexture2D* texture = flipbookWrapper->GetKeyframeTexture(index);
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
FOdysseyFlipbookEditorToolkit::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
	UTexture2D* texture = iSprite->GetSourceTexture();

	UPaperFlipbook* flipbook = mData->FlipbookWrapper()->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				RemoveEditingObject(iOldTexture);

			if (texture)
				AddEditingObject(texture);
		}
	}
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
	for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (!sprite)
			continue;

		AddEditingObject(sprite);
	}

    FAssetEditorToolkit::SaveAsset_Execute();

	for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (!sprite)
			continue;

		RemoveEditingObject(sprite);
	}

    // mData->FlipbookWrapper()->MarkPackageDirty(); //PATCH:
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
	return GetLabelForObject(mData->FlipbookWrapper()->Flipbook());
}


FText
FOdysseyFlipbookEditorToolkit::GetToolkitToolTipText() const
{
	return GetToolTipTextForObject(mData->FlipbookWrapper()->Flipbook());
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

bool
FOdysseyFlipbookEditorToolkit::CanReimport() const
{
	return false;
}

bool
FOdysseyFlipbookEditorToolkit::CanReimport(UObject* EditingObject) const
{
	return false;
}

#undef LOCTEXT_NAMESPACE