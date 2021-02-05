// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditor.h"

#include "OdysseyFlipbookWrapper.h"

#include "OdysseyFlipbookEditorController.h"
#include "OdysseyFlipbookEditorData.h"
#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyFlipbookEditorToolkit.h"

#include "Types/NavigationMetaData.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditor::~FOdysseyFlipbookEditor()
{
	mData->FlipbookWrapper()->OnSpriteTextureChanged().Remove(mOnSpriteTextureChangedHandle);
}

FOdysseyFlipbookEditor::FOdysseyFlipbookEditor() :
	FOdysseyPainterEditor()
{
}

void
FOdysseyFlipbookEditor::InitWithFlipbook(UPaperFlipbook* iFlipbook)
{
	FOdysseyPainterEditor::Init();

	//----

	TSharedPtr<FOdysseyFlipbookWrapper> flipbookWrapper = MakeShareable(new FOdysseyFlipbookWrapper(iFlipbook));
	mOnSpriteTextureChangedHandle = flipbookWrapper->OnSpriteTextureChanged().AddRaw(this, &FOdysseyFlipbookEditor::OnSpriteTextureChanged);

	//----

	mData = MakeShareable(new FOdysseyFlipbookEditorData(flipbookWrapper, GetToolkit()));
	mGUI = MakeShareable(new FOdysseyFlipbookEditorGUI());
	mController = MakeShareable(new FOdysseyFlipbookEditorController(mData, mGUI));

	mController->OnSpriteCreated().BindRaw(this, &FOdysseyFlipbookEditor::OnSpriteCreated);
    mController->OnTextureCreated().BindRaw(this, &FOdysseyFlipbookEditor::OnTextureCreated);
    mController->OnKeyframeRemoved().BindRaw(this, &FOdysseyFlipbookEditor::OnKeyframeRemoved);

	mData->Init();
	mGUI->Init(mData, mController);
	mController->Init(GetToolkit()->GetToolkitCommands());
}

void
FOdysseyFlipbookEditor::OnToolkitInitialized()
{
	TSharedPtr<SDockTab> OwnerTab = GetToolkit()->GetTabManager()->GetOwnerTab();
	TSharedPtr<SWindow> parentWindow = NULL;
	if (OwnerTab.IsValid())
	{
		parentWindow = FSlateApplication::Get().FindWidgetWindow(OwnerTab.ToSharedRef());
	}
	else
	{
		parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
	}

	SetTimelineNavigationShortcuts(parentWindow);
}

void
FOdysseyFlipbookEditor::SetTimelineNavigationShortcuts(TSharedPtr<SWidget> iWidget)
{
	TSharedPtr<FNavigationMetaData> navigationMetaData = MakeShareable(new FNavigationMetaData());
	navigationMetaData->SetNavigationCustom(EUINavigation::Left, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Right, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Next, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Previous, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	iWidget->AddMetadata(navigationMetaData.ToSharedRef());
}

void
FOdysseyFlipbookEditor::OnSpriteCreated(UPaperSprite* iSprite)
{
	GetToolkit()->AddEditingObject(iSprite);
}


void
FOdysseyFlipbookEditor::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
	UTexture2D* texture = iSprite->GetSourceTexture();

	UPaperFlipbook* flipbook = mData->FlipbookWrapper()->Flipbook();
	for (int i = 0; i < flipbook->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (sprite == iSprite)
		{
			if (iOldTexture)
				GetToolkit()->RemoveEditingObject(iOldTexture);

			if (texture)
				GetToolkit()->AddEditingObject(texture);
		}
	}
}

void
FOdysseyFlipbookEditor::OnTextureCreated(UTexture2D* iTexture)
{
	GetToolkit()->AddEditingObject(iTexture);
}

void
FOdysseyFlipbookEditor::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
{
	if (!iKeyframe.Sprite)
		return;

	GetToolkit()->RemoveEditingObject(iKeyframe.Sprite);

	UTexture2D* texture = iKeyframe.Sprite->GetSourceTexture();
	if (!texture)
		return;

	GetToolkit()->RemoveEditingObject(texture);
}

bool
FOdysseyFlipbookEditor::OnCloseRequested()
{
	mData->PaintEngine()->Flush();
	mData->SyncTextureWithSurfaceBlock();
	mData->ApplyPropertiesBackup();

	//TODO: Move in the right place
    if (mData->LayerStack())
	{
		mData->LayerStack()->mDrawingUndo->Clear();
	}
    return true;
}

const TSharedRef<FTabManager::FLayout>&
FOdysseyFlipbookEditor::CreateLayout() const
{
	return mGUI->GetLayout();
}

const TArray<TSharedPtr<FExtender>>&
FOdysseyFlipbookEditor::CreateMenuExtenders() const
{
	return mController->GetMenuExtenders();
}

TSharedPtr<FWorkspaceItem>
FOdysseyFlipbookEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyFlipbookEditor", "Odyssey Flipbook Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	mGUI->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

void
FOdysseyFlipbookEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
	mGUI->UnregisterTabSpawners(iTabManager);
}
#undef LOCTEXT_NAMESPACE