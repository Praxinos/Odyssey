// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorGUI.h"

#include "OdysseyFlipbookEditorTimelineTab.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"
#include "Framework/Docking/LayoutExtender.h"
#include "FlipbookEditor/OdysseyFlipbookEditorExtension.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "SOdysseyFlipbookTimelineView.h"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorGUI
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorGUI::~FOdysseyFlipbookEditorGUI()
{
}

FOdysseyFlipbookEditorGUI::FOdysseyFlipbookEditorGUI(FOdysseyFlipbookEditorExtension* iExtension)
    : mExtension(iExtension)
{
}

void
FOdysseyFlipbookEditorGUI::Initialize()
{
    CreateTabs();
}

void
FOdysseyFlipbookEditorGUI::Finalize()
{
}

void
FOdysseyFlipbookEditorGUI::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{    
    TSharedRef<FTabManager::FSplitter> mainVerticalSplitter = iBuilder.GetSplitter("MainVerticalSplitter");    
    TSharedRef<FTabManager::FStack> flipbookTimelineStack = iBuilder.CreateStack("FlipbookTimelineStack");
    flipbookTimelineStack->SetHideTabWell(false);
    flipbookTimelineStack->SetSizeCoefficient(0.2f);
    flipbookTimelineStack->AddTab(FOdysseyFlipbookEditorTimelineTab::StaticId(), ETabState::OpenedTab);
    mainVerticalSplitter->Split
    (
        flipbookTimelineStack
    );
}

void
FOdysseyFlipbookEditorGUI::OnFlipbookChanged()
{
    //Override displayed texture only if a flipbook is being edited
    if (mExtension->GetFlipbook())
    {
        TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
        TAttribute<UTexture*> textureAttr = TAttribute<UTexture*>::CreateLambda(
            [this]() -> UTexture*
            {
                TSharedPtr<FOdysseyFlipbookEditorTimelineTab> timelineTab = mExtension->GetEditor()->FindTab<FOdysseyFlipbookEditorTimelineTab>();

                if (!timelineTab->Timeline())
                    return nullptr;

                if (timelineTab->Timeline()->IsScrubbing())
                    return mExtension->PreviewTexture();

                TSharedPtr<FOdysseyPainterEditorSource> source = mExtension->GetEditor()->GetSource();
                if (!source)
                    return nullptr;

                return source->DisplayTexture();
            }
        );

        viewportTab->SetTexture(textureAttr);
    }
    else
    {
        TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = mExtension->GetEditor()->FindTab<FOdysseyPainterEditorViewportTab>();
        viewportTab->SetDefaultTexture();
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyFlipbookEditorGUI::ExtendLevelEditorLayout(FLayoutExtender& Extender)
{   
    Extender.ExtendLayout(FTabId(TEXT("LevelEditorSelectionDetails")), ELayoutExtensionPosition::Above, FTabManager::FTab(FOdysseyFlipbookEditorTimelineTab::StaticId(), ETabState::ClosedTab));
}

void
FOdysseyFlipbookEditorGUI::CreateTabs()
{
    //ADD NEW TABS
    TSharedRef<FOdysseyFlipbookEditorTimelineTab> timelineTab = MakeShared<FOdysseyFlipbookEditorTimelineTab>(mExtension);

    timelineTab->ShouldOpenByDefault(true);

    mExtension->GetEditor()->AddTab(timelineTab);
}

/* TSharedRef<FTabManager::FSplitter>
FOdysseyFlipbookEditorGUI::CreateMainSection()
{
    return FOdysseyTextureEditorGUI::CreateMainSection()
        // Timeline + Notes
        ->Split
        (
            FTabManager::NewStack()
            // Timeline
            ->AddTab(mTimelineTab->ID(), ETabState::OpenedTab)
            ->SetHideTabWell(false)
            ->SetSizeCoefficient(0.2f)
        );
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<FOdysseyFlipbookEditorTimelineTab>&
FOdysseyFlipbookEditorGUI::GetTimelineTab()
{
    return mTimelineTab;
}
