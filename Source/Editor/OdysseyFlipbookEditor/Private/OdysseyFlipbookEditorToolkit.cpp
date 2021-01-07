// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorToolkit.h"

#include "OdysseyFlipbookEditorController.h"
#include "OdysseyFlipbookEditorData.h"
#include "OdysseyFlipbookEditorGUI.h"

#include "Types/NavigationMetaData.h"

#include "IOdysseyFlipbookEditorModule.h"

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
		UPaperSprite* sprite = flipbookWrapper->GetKeyframeSprite(index);
        if (!sprite)
            continue;

		objectsToEdit.Add(sprite);

		UTexture2D* texture = flipbookWrapper->GetKeyframeTexture(index);
        if (!texture)
            continue;
            
        objectsToEdit.Add(texture);
	}

	FOdysseyPainterEditorToolkit::InitPainterEditorToolkit(iMode, iInitToolkitHost, iAppIdentifier, objectsToEdit);

	TSharedPtr<SDockTab> OwnerTab = GetTabManager()->GetOwnerTab();
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

	//The following does not work but keeping it for record
	/* SetTimelineNavigationShortcuts(mGUI->GetLayerStackTab());
	SetTimelineNavigationShortcuts(mGUI->GetTimelineTab());
    SetTimelineNavigationShortcuts(mGUI->GetViewportTab());
    SetTimelineNavigationShortcuts(mGUI->GetBrushSelectorTab());
    SetTimelineNavigationShortcuts(mGUI->GetMeshSelectorTab());
    SetTimelineNavigationShortcuts(mGUI->GetBrushExposedParametersTab());
    SetTimelineNavigationShortcuts(mGUI->GetColorSelectorTab());
    SetTimelineNavigationShortcuts(mGUI->GetColorSlidersTab());
    SetTimelineNavigationShortcuts(mGUI->GetStrokeOptionsTab());
    SetTimelineNavigationShortcuts(mGUI->GetPerformanceOptionsTab());
    //SetTimelineNavigationShortcuts(mGUI->GetUndoHistoryTab());
    SetTimelineNavigationShortcuts(mGUI->GetTopTab());
    SetTimelineNavigationShortcuts(mGUI->GetToolsTab()); */
}

void
FOdysseyFlipbookEditorToolkit::SetTimelineNavigationShortcuts(TSharedPtr<SWidget> iWidget)
{
	TSharedPtr<FNavigationMetaData> navigationMetaData = MakeShareable(new FNavigationMetaData());
	navigationMetaData->SetNavigationCustom(EUINavigation::Left, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Right, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Next, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Previous, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mGUI->GetTimelineTab().ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	iWidget->AddMetadata(navigationMetaData.ToSharedRef());
}

void
FOdysseyFlipbookEditorToolkit::OnSpriteCreated(UPaperSprite* iSprite)
{
	AddEditingObject(iSprite);
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
	/* mData->PaintEngine()->Flush();
	mData->SyncTextureWithSurfaceBlock();
	mData->ApplyPropertiesBackup(); */

	//Small trick
	//We want to save all sprites, but we don't want to be considered the actual sprite editor
	//So we set ourselves as editing all sprites just before saving and cancel this just after saving
	/* for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (!sprite)
			continue;

		AddEditingObject(sprite);
	} */

    FAssetEditorToolkit::SaveAsset_Execute();

	// mData->PrepareTextureProperties();

	/* for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UPaperSprite* sprite = mData->FlipbookWrapper()->GetKeyframeSprite(i);
		if (!sprite)
			continue;

		RemoveEditingObject(sprite);
	} */
}

void
FOdysseyFlipbookEditorToolkit::SaveAssetAs_Execute()
{	
	/* for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UTexture2D* texture = mData->FlipbookWrapper()->GetKeyframeTexture(i);
		if (!texture)
			continue;

		RemoveEditingObject(texture);
	} */
	/* mData->PaintEngine()->Flush();
	mData->SyncTextureWithSurfaceBlock();
	mData->ApplyPropertiesBackup(); */

	//PATCH: Intercept Open request to open the asset in iliad instead of the default editor
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	FDelegateHandle openAssetHandle = AssetEditorSubsystem->OnAssetEditorRequestedOpen().AddRaw(this, &FOdysseyFlipbookEditorToolkit::OpenAsset);

    FAssetEditorToolkit::SaveAssetAs_Execute();

	// mData->PrepareTextureProperties();

	AssetEditorSubsystem->OnAssetEditorRequestedOpen().Remove(openAssetHandle);

	/* for (int i = 0; i < mData->FlipbookWrapper()->Flipbook()->GetNumKeyFrames(); i++)
	{
		UTexture2D* texture = mData->FlipbookWrapper()->GetKeyframeTexture(i);
		if (!texture)
			continue;

		AddEditingObject(texture);
	} */
}

void
FOdysseyFlipbookEditorToolkit::OpenAsset(UObject* iObject)
{
	UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(iObject);
	IOdysseyFlipbookEditorModule* odysseyFlipbookEditorModule = &FModuleManager::GetModuleChecked<IOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
	odysseyFlipbookEditorModule->CreateOdysseyFlipbookEditor(EToolkitMode::Standalone, NULL, flipbook);
}

bool
FOdysseyFlipbookEditorToolkit::OnRequestClose()
{
	mData->PaintEngine()->Flush();
	mData->SyncTextureWithSurfaceBlock();
	mData->ApplyPropertiesBackup();

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