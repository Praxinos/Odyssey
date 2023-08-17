// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditor.h"

/*
#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookEditorTimelineTab.h"
#include "OdysseySurfaceTexture2D.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"

#include <ULIS>

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditor::~FOdysseyFlipbookEditor()
{
	mFlipbookWrapper->OnSpriteTextureChanged().RemoveAll(this);
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor() :
	FOdysseyTextureEditor(),
	mFlipbookWrapper(nullptr),
	mPreviewSurface(nullptr),
	mGUI(nullptr)
{
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook) :
	FOdysseyTextureEditor(),
	mFlipbookWrapper(MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook))),
	mPreviewSurface(nullptr),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyFlipbookEditor::InitData(UObject* iEditedObject)
{
	//--- Add Edited Objects

	UPaperFlipbook* flipbook = mFlipbookWrapper->Flipbook();
	TSharedPtr<FOdysseyFlipbookWrapper> flipbookWrapper = MakeShareable(new FOdysseyFlipbookWrapper(flipbook));

	for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
	{
		UPaperSprite* sprite = flipbookWrapper->GetKeyframeSprite(index);
        if (!sprite)
            continue;

		AddEditedObject(sprite);

		UTexture2D* texture = flipbookWrapper->GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        AddEditedObject(texture);
	}

	//----

	mFlipbookWrapper->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditor::OnSpriteTextureChanged);
	
	//----

    //We don't need to initialize Texture if there is no keyFrames
    if (mFlipbookWrapper->Flipbook()->GetNumKeyFrames() > 0)
	{
		UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(0);
		TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
		SetSource(source);
	}
	
	FOdysseyTextureEditor::InitData(iEditedObject);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters


TSharedPtr<FOdysseyFlipbookWrapper>&
FOdysseyFlipbookEditor::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

UTexture*
FOdysseyFlipbookEditor::PreviewTexture()
{
	return mPreviewSurface.Texture();
}

void
FOdysseyFlipbookEditor::PreviewTexture(UTexture2D* iTexture)
{
	return mPreviewSurface.Texture(iTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyFlipbookEditorGUI*
FOdysseyFlipbookEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI(this));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyFlipbookEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyFlipbookEditor", "Odyssey Flipbook Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyFlipbookEditor::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = mFlipbookWrapper->GetKeyframeTexture(iKeyframeIndex);
	if (GetGUI()->GetTimelineTab()->Timeline()->IsScrubbing())
	{
		mPreviewSurface.Texture(texture);
		return;
	}

	TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
	SetSource(source);
}

void
FOdysseyFlipbookEditor::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
	UPaperFlipbook* flipbook = mFlipbookWrapper->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				RemoveEditedObject(iOldTexture);

			if (texture)
				AddEditedObject(texture);
		}
	}

	int32 index = GetGUI()->GetTimelineTab()->Timeline()->GetCurrentKeyframeIndex();

	UPaperSprite* sprite = mFlipbookWrapper->GetKeyframeSprite(index);
	if (sprite != iSprite)
		return;

	SetTextureAtKeyframeIndex(index);
}

#undef LOCTEXT_NAMESPACE
*/