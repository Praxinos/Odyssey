// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorFlipbookTimelineTab.h"

#include "OdysseyPainterEditorFlipbookCommands.h"
#include "OdysseyPainterEditor.h"
#include "SOdysseyFlipbookTimelineView.h"
#include "Types/NavigationMetaData.h"
#include "OdysseyPainterEditorTextureSource.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "PaperSprite.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

const FName&
FOdysseyPainterEditorFlipbookTimelineTab::StaticId()
{
    static FName Id = TEXT("OdysseyFlipbookEditor_Timeline");  //Dont change, Old Id for retro compatibility
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorFlipbookTimelineTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorFlipbookTimelineTab::~FOdysseyPainterEditorFlipbookTimelineTab()
{
}

FOdysseyPainterEditorFlipbookTimelineTab::FOdysseyPainterEditorFlipbookTimelineTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "timeline-tab.name", "Timeline" ),
                        FSlateIcon( "OdysseyStyle", "FlipbookEditor.Layers16" )) //TODO: Timeline Icon
    , mEditor(iEditor)
    , mTimeline(nullptr)
{
}

const FName&
FOdysseyPainterEditorFlipbookTimelineTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorFlipbookTimelineTab::CreateWidget()
{
    mTimeline = SNew(SOdysseyFlipbookTimelineView)
        .Flipbook(this, &FOdysseyPainterEditorFlipbookTimelineTab::GetFlipbook)
        .OnScrubStarted_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineScrubStarted)
        .OnScrubStopped_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineScrubStopped)
        .OnCurrentKeyframeChanged_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineCurrentKeyframeChanged)
        .OnFlipbookChanged_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnFlipbookChanged)
        .OnSpriteCreated_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnSpriteCreated)
        .OnTextureCreated_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnTextureCreated)
        .OnKeyframeRemoved_Raw(this, &FOdysseyPainterEditorFlipbookTimelineTab::OnKeyframeRemoved);

    return mTimeline;
}

void
FOdysseyPainterEditorFlipbookTimelineTab::BindShortcuts(FBaseToolkit* iToolkit)
{
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorFlipbookCommands& flipbookEditorCommands = FOdysseyPainterEditorFlipbookCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorViewportTab::__VA_ARGS__ ), FCanExecuteAction() );

    // Insert Shortcuts Here

    #undef MAP_ACTION

    BindNavigationShortcuts(iToolkit);

    if (mTimeline)
        mTimeline->BindCommands(toolkitCommands);
}

bool
FOdysseyPainterEditorFlipbookTimelineTab::CanOpen() const
{
    return !!GetFlipbook();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyFlipbookTimelineView>
FOdysseyPainterEditorFlipbookTimelineTab::Timeline()
{
    return mTimeline;
}

UPaperFlipbook*
FOdysseyPainterEditorFlipbookTimelineTab::GetFlipbook() const
{
    return Cast<UPaperFlipbook>(mEditor->GetEditedObject());
}



//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineCurrentKeyframeChanged(int32 iKeyframe)
{
    SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineScrubStarted()
{
    SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnTimelineScrubStopped()
{
    SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnFlipbookChanged()
{
    SetTextureAtKeyframeIndex(mTimeline->GetCurrentKeyframeIndex());
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnSpriteCreated(UPaperSprite* iSprite)
{
    mEditor->AddEditedObject(iSprite);
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnTextureCreated(UTexture2D* iTexture, FOdysseyTextureConfiguration iTextureConfiguration)
{
    mEditor->AddEditedObject(iTexture);
}

void
FOdysseyPainterEditorFlipbookTimelineTab::OnKeyframeRemoved(FPaperFlipbookKeyFrame& iKeyframe)
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
FOdysseyPainterEditorFlipbookTimelineTab::SetTextureAtKeyframeIndex(int32 iKeyframeIndex)
{

    UObject* editedObject = mEditor->GetEditedObject();
    if (!editedObject)
        return;

    if (!editedObject->IsA<UPaperFlipbook>())
        return;

    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(editedObject);

    UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(flipbook, iKeyframeIndex);
    if (texture)
    {
        TSharedPtr<FOdysseyPainterEditorTextureSource> source = MakeShared<FOdysseyPainterEditorTextureSource>(texture);
        mEditor->SetSource(source);
    }
    else
    {
        mEditor->SetSource(nullptr);
    }
}

void
FOdysseyPainterEditorFlipbookTimelineTab::BindNavigationShortcuts(FBaseToolkit* iToolkit)
{
    if (!iToolkit->IsAssetEditor())
        return;

    if (!mTimeline)
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
