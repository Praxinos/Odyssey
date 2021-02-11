// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorTimelineTab.h"

#include "OdysseyFlipbookEditor.h"
#include "SOdysseyFlipbookTimelineView.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorTimelineTab"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorTimelineTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorTimelineTab::~FOdysseyFlipbookEditorTimelineTab()
{
}

FOdysseyFlipbookEditorTimelineTab::FOdysseyFlipbookEditorTimelineTab(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyFlipbookEditor_Timeline"),
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
FOdysseyFlipbookEditorTimelineTab::BindShortcuts()
{
    const TSharedRef<FUICommandList>& toolkitCommands = mEditor->Toolkit()->GetToolkitCommands();
    const FOdysseyFlipbookEditorCommands& flipbookEditorCommands = FOdysseyFlipbookEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorViewportTab::__VA_ARGS__ ), FCanExecuteAction() );

    mTimeline->BindCommands(toolkitCommands);

    #undef MAP_ACTION
}

void
FOdysseyFlipbookEditorTimelineTab::OnToolkitInitialized()
{
	BindNavigationShortcuts();
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
}

void
FOdysseyFlipbookEditorTimelineTab::OnTimelineScrubStopped()
{
	//TODO: unlock paintengine
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
	mEditor->Toolkit()->AddEditingObject(iSprite);
}

void
FOdysseyFlipbookEditorTimelineTab::OnTextureCreated(UTexture2D* iTexture)
{
	mEditor->Toolkit()->AddEditingObject(iTexture);
}

void
FOdysseyFlipbookEditorTimelineTab::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
{
	if (!iKeyframe.Sprite)
		return;

	mEditor->Toolkit()->RemoveEditingObject(iKeyframe.Sprite);

	UTexture2D* texture = iKeyframe.Sprite->GetSourceTexture();
	if (!texture)
		return;

	mEditor->Toolkit()->RemoveEditingObject(texture);
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
        mEditor->GetGUI()->GetTextureDetailsTab()->SetTexture(texture);
		return;
	}

    mEditor->Texture(texture);
}

void
FOdysseyFlipbookEditorTimelineTab::BindNavigationShortcuts()
{
	TSharedPtr<SDockTab> OwnerTab = mEditor->Toolkit()->GetTabManager()->GetOwnerTab();
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
