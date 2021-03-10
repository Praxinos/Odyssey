// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditor.h"

#include "OdysseyFlipbookEditorGUI.h"
#include "OdysseyFlipbookEditorTimelineTab.h"
#include "OdysseySurfaceReadOnly.h"
#include "SOdysseyFlipbookTimelineView.h"

#include <ULIS3>

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditor::~FOdysseyFlipbookEditor()
{
	mFlipbookWrapper->OnSpriteTextureChanged().RemoveAll(this);

	if (mPreviewSurface) {
		delete mPreviewSurface;
		mPreviewSurface = nullptr;
	}
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor() :
	FOdysseyTextureEditor(),
	mFlipbookWrapper(nullptr),
	mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr)),
	mGUI(nullptr)
{
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor(UPaperFlipbook* iFlipbook) :
	FOdysseyTextureEditor(),
	mFlipbookWrapper(MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook))),
	mPreviewSurface(new FOdysseySurfaceReadOnly(nullptr)),
	mGUI(nullptr)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyFlipbookEditor::InitData()
{
	FOdysseyTextureEditor::InitData();

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
    	Texture(mFlipbookWrapper->GetKeyframeTexture(0));
	}
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters


TSharedPtr<FOdysseyFlipbookWrapper>&
FOdysseyFlipbookEditor::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

FOdysseySurfaceReadOnly*
FOdysseyFlipbookEditor::PreviewSurface()
{
	return mPreviewSurface;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters


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
		PreviewSurface()->Texture(texture);
		return;
	}

    Texture(texture);
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
