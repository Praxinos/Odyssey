// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FlipbookEditor/OdysseyFlipbookEditorExtension.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "FlipbookEditor/OdysseyFlipbookEditorGUI.h"
#include "FlipbookEditor/OdysseyFlipbookEditorTimelineTab.h"
#include "Widgets/SOdysseyFlipbookTimelineView.h"
#include "TextureEditor/OdysseyTextureEditorSource.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorExtension"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyFlipbookEditorExtension::~FOdysseyFlipbookEditorExtension()
{
}

FOdysseyFlipbookEditorExtension::FOdysseyFlipbookEditorExtension(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorExtension(iEditor)
	, mPreviewSurface(nullptr)
	, mGUI(nullptr)
{
}

void
FOdysseyFlipbookEditorExtension::Initialize()
{
}

void
FOdysseyFlipbookEditorExtension::Finalize()
{
	SetFlipbook(nullptr);
}

void
FOdysseyFlipbookEditorExtension::SetFlipbook(UPaperFlipbook* iFlipbook)
{
	if (iFlipbook == GetFlipbook())
		return;

	if (!iFlipbook)
	{
		UPaperFlipbook* flipbook = GetFlipbook();
		if (flipbook)
		{
			//Remove all additional Edited Objects (Sprites and Textures)
			for (int32 index = 0; index < flipbook->GetNumKeyFrames(); ++index)
			{
				UPaperSprite* sprite = mFlipbookWrapper.GetKeyframeSprite(index);
				if (!sprite)
					continue;

				GetEditor()->RemoveEditedObject(sprite);

				UTexture2D* texture = mFlipbookWrapper.GetKeyframeTexture(index);
				if (!texture)
					continue;
					
				GetEditor()->RemoveEditedObject(texture);
			}
		}

		GetEditor()->SetSource(nullptr);
		mFlipbookWrapper.SetFlipbook(nullptr);
		mFlipbookWrapper.OnSpriteTextureChanged().RemoveAll(this);
		return;
	}
	
	mFlipbookWrapper.SetFlipbook(iFlipbook);

	//Find all additional Edited Objects (Sprites and Textures)
	for (int32 index = 0; index < iFlipbook->GetNumKeyFrames(); ++index)
	{
		UPaperSprite* sprite = mFlipbookWrapper.GetKeyframeSprite(index);
        if (!sprite)
            continue;

		GetEditor()->AddEditedObject(sprite);

		UTexture2D* texture = mFlipbookWrapper.GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        GetEditor()->AddEditedObject(texture);
	}

	// Set Sprite Changed Callback
	mFlipbookWrapper.OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditorExtension::OnSpriteTextureChanged);

	//TODO: Activate GUI

	//Set Texture Source if needed
	if (iFlipbook->GetNumKeyFrames() > 0)
	{
		UTexture2D* texture = mFlipbookWrapper.GetKeyframeTexture(0);
		TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
		GetEditor()->SetSource(source);
	}
}

UPaperFlipbook*
FOdysseyFlipbookEditorExtension::GetFlipbook()
{
	return mFlipbookWrapper.GetFlipbook();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyFlipbookWrapper&
FOdysseyFlipbookEditorExtension::FlipbookWrapper()
{
	return mFlipbookWrapper;
}

UTexture*
FOdysseyFlipbookEditorExtension::PreviewTexture()
{
	return mPreviewSurface.Texture();
}

void
FOdysseyFlipbookEditorExtension::PreviewTexture(UTexture2D* iTexture)
{
	return mPreviewSurface.Texture(iTexture);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyFlipbookEditorGUI*
FOdysseyFlipbookEditorExtension::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShared<FOdysseyFlipbookEditorGUI>(this);
	return mGUI.Get();
}

/* TSharedPtr<FWorkspaceItem>
FOdysseyFlipbookEditorExtension::RegisterTabSpawners(const TSharedRef<FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyFlipbookEditor", "Odyssey Animation2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
} */

void
FOdysseyFlipbookEditorExtension::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = mFlipbookWrapper.GetKeyframeTexture(iKeyframeIndex);
	if (GetGUI()->GetTimelineTab()->Timeline()->IsScrubbing())
	{
		mPreviewSurface.Texture(texture);
		return;
	}

	TSharedPtr<FOdysseyTextureEditorSource> source = MakeShared<FOdysseyTextureEditorSource>(texture);
	GetEditor()->SetSource(source);
}

void
FOdysseyFlipbookEditorExtension::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
	UPaperFlipbook* flipbook = mFlipbookWrapper.GetFlipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mFlipbookWrapper.GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				GetEditor()->RemoveEditedObject(iOldTexture);

			if (texture)
				GetEditor()->AddEditedObject(texture);
		}
	}

	int32 index = GetGUI()->GetTimelineTab()->Timeline()->GetCurrentKeyframeIndex();

	UPaperSprite* sprite = mFlipbookWrapper.GetKeyframeSprite(index);
	if (sprite != iSprite)
		return;

	SetTextureAtKeyframeIndex(index);
}

#undef LOCTEXT_NAMESPACE