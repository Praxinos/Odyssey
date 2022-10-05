// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorTimelineTab.h"

#include "OdysseyFlipbookEditor.h"
#include "OdysseyFlipbookEditorCommands.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "Types/NavigationMetaData.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorTimelineTab"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorTimelineTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorTimelineTab::~FOdysseyFlipbookEditorTimelineTab()
{
}

FOdysseyFlipbookEditorTimelineTab::FOdysseyFlipbookEditorTimelineTab(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyFlipbookEditor_Timeline"),
                            LOCTEXT( "OdysseyFlipbookEditorTimelineTab", "Timeline" ),
                            FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" )) //TODO: Timeline Icon
    , mEditor(iEditor)
    , mTimeline(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyFlipbookEditorTab interface

TSharedPtr<SWidget>
FOdysseyFlipbookEditorTimelineTab::CreateWidget()
{
	mTimeline = SNew(SOdysseyFlipbookTimelineView)
		//.FlipbookWrapper_Raw(this, &FOdysseyFlipbookEditorTimelineTab::FlipbookWrapper) //TODO:
		.FlipbookWrapper(FlipbookWrapper())
		.OnScrubStarted_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnTimelineScrubStarted)
		.OnScrubStopped_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnTimelineScrubStopped)
		.OnCurrentKeyframeChanged_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnTimelineCurrentKeyframeChanged)
		.OnFlipbookChanged_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnFlipbookChanged)
		.OnSpriteCreated_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnSpriteCreated)
		.OnTextureCreated_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnTextureCreated)
		.OnKeyframeRemoved_Raw(this, &FOdysseyFlipbookEditorTimelineTab::OnKeyframeRemoved);

    return mTimeline;
}

void
FOdysseyFlipbookEditorTimelineTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyFlipbookEditorCommands& flipbookEditorCommands = FOdysseyFlipbookEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorViewportTab::__VA_ARGS__ ), FCanExecuteAction() );

	// Insert Shortcuts Here

    #undef MAP_ACTION

	BindNavigationShortcuts(iToolkit);
	mTimeline->BindCommands(toolkitCommands);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyFlipbookTimelineView>
FOdysseyFlipbookEditorTimelineTab::Timeline()
{
    return mTimeline;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

TSharedPtr<FOdysseyFlipbookWrapper>
FOdysseyFlipbookEditorTimelineTab::FlipbookWrapper() const
{
    return mEditor->FlipbookWrapper();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyFlipbookEditorTimelineTab::OnTimelineCurrentKeyframeChanged(int32 iKeyframe)
{
	SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyFlipbookEditorTimelineTab::OnTimelineScrubStarted()
{
	//TODO: lock paintengine, to avoid drawing while scrubbing
	SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyFlipbookEditorTimelineTab::OnTimelineScrubStopped()
{
	//TODO: unlock paintengine, to avoid drawing while scrubbing
	SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());

	//Cleanup Preview Surface
	mEditor->PreviewSurface()->Texture(NULL);
}

void
FOdysseyFlipbookEditorTimelineTab::OnFlipbookChanged()
{
	SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyFlipbookEditorTimelineTab::OnSpriteCreated(UPaperSprite* iSprite)
{
	mEditor->AddEditedObject(iSprite);
}

void
FOdysseyFlipbookEditorTimelineTab::OnTextureCreated(UTexture2D* iTexture)
{
	mEditor->AddEditedObject(iTexture);
}

void
FOdysseyFlipbookEditorTimelineTab::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
{
	if (!iKeyframe.Sprite)
		return;

	mEditor->RemoveEditedObject(iKeyframe.Sprite);

	UTexture2D* texture = iKeyframe.Sprite->GetSourceTexture();
	if (!texture)
		return;

	mEditor->RemoveEditedObject(texture);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Methods

void
FOdysseyFlipbookEditorTimelineTab::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{
	UTexture2D* texture = mEditor->FlipbookWrapper()->GetKeyframeTexture(iKeyframeIndex);

    //TODO: Instead of going through the GUI, make a Player class in the data and get the condition from there
	if (mTimeline->IsScrubbing())
	{
		mEditor->PreviewSurface()->Texture(texture);
		return;
	}

    mEditor->SetTexture(texture);
}

void
FOdysseyFlipbookEditorTimelineTab::BindNavigationShortcuts(FBaseToolkit* iToolkit)
{
	if (!iToolkit->IsAssetEditor())
		return;

	FAssetEditorToolkit* toolkit = static_cast<FAssetEditorToolkit*>(iToolkit);
	TSharedPtr<SDockTab> OwnerTab = toolkit->GetTabManager()->GetOwnerTab();
	TSharedPtr<SWindow> parentWindow = NULL;
	if (OwnerTab.IsValid())
	{
		parentWindow = FSlateApplication::Get().FindWidgetWindow(OwnerTab.ToSharedRef());
	}
	else
	{
		parentWindow = FGlobalTabmanager::Get()->GetRootWindow();
	}

	TSharedPtr<FNavigationMetaData> navigationMetaData = MakeShareable(new FNavigationMetaData());
	navigationMetaData->SetNavigationCustom(EUINavigation::Left, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mTimeline.ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Right, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mTimeline.ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Next, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mTimeline.ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	navigationMetaData->SetNavigationCustom(EUINavigation::Previous, EUINavigationRule::Custom, FNavigationDelegate::CreateSP(mTimeline.ToSharedRef(), &SOdysseyFlipbookTimelineView::OnArrowNavigation));
	parentWindow->AddMetadata(navigationMetaData.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
