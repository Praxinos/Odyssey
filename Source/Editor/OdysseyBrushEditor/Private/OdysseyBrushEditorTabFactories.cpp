// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushEditorTabFactories.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Notifications/SErrorText.h"
#include "EditorStyleSet.h"

#include "OdysseyBrushEditorSharedTabFactories.h"
#include "OdysseyBrushEditorTabs.h"
#include "SOdysseyTimelineEditor.h"
#include "Debugging/SOdysseyBrushDebuggingView.h"
#include "SOdysseyInspector.h"
#include "SOdysseySCSEditor.h"
#include "SOdysseySCSEditorViewport.h"
#include "SOdysseyBrushPalette.h"
#include "SOdysseyBrushBookmarks.h"
#include "SOdysseyBrushPreferencesOverrides.h"
#include "SOdysseyBrushPreview.h"
#include "SOdysseyBrushTestArea.h"
#include "SOdysseyBrushInternalExposed.h"
#include "FindInOdysseyBrush.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SMyOdysseyBrush.h"
#include "SOdysseyReplaceNodeReferences.h"
#include "Widgets/Input/SHyperlink.h"
#include "BlueprintEditorSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorTabFactories"

void FGraphEditorSummoner::OnTabActivated(TSharedPtr<SDockTab> Tab) const
{
    TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
    OdysseyBrushEditorPtr.Pin()->OnGraphEditorFocused(GraphEditor);
}

void FGraphEditorSummoner::OnTabBackgrounded(TSharedPtr<SDockTab> Tab) const
{
    TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
    OdysseyBrushEditorPtr.Pin()->OnGraphEditorBackgrounded(GraphEditor);
}

void FGraphEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
    TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());
    GraphEditor->NotifyGraphChanged();
}

void FGraphEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
    TSharedRef<SGraphEditor> GraphEditor = StaticCastSharedRef<SGraphEditor>(Tab->GetContent());

    FVector2D ViewLocation;
    float ZoomAmount;
    GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);

    UEdGraph* Graph = Payload->IsValid() ? FTabPayload_UObject::CastChecked<UEdGraph>(Payload) : nullptr;

    if (Graph && OdysseyBrushEditorPtr.Pin()->IsGraphInCurrentOdysseyBrush(Graph))
    {
        // Don't save references to external graphs.
        OdysseyBrushEditorPtr.Pin()->GetBlueprintObj()->LastEditedDocuments.Add(FEditedDocumentInfo(Graph, ViewLocation, ZoomAmount));
    }
}

FGraphEditorSummoner::FGraphEditorSummoner(TSharedPtr<class FOdysseyBrushEditor> InOdysseyBrushEditorPtr, FOnCreateGraphEditorWidget CreateGraphEditorWidgetCallback) : FDocumentTabFactoryForObjects<UEdGraph>(FOdysseyBrushEditorTabs::GraphEditorID, InOdysseyBrushEditorPtr)
, OdysseyBrushEditorPtr(InOdysseyBrushEditorPtr)
, OnCreateGraphEditorWidget(CreateGraphEditorWidgetCallback)
{

}

TSharedRef<SWidget> FGraphEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
    check(Info.TabInfo.IsValid());
    return OnCreateGraphEditorWidget.Execute(Info.TabInfo.ToSharedRef(), DocumentID);
}

const FSlateBrush* FGraphEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UEdGraph* DocumentID) const
{
    return FOdysseyBrushEditor::GetGlyphForGraph(DocumentID, false);
}

TSharedRef<FGenericTabHistory> FGraphEditorSummoner::CreateTabHistoryNode(TSharedPtr<FTabPayload> Payload)
{
    return MakeShareable(new FGraphTabHistory(SharedThis(this), Payload));
}

void FTimelineEditorSummoner::OnTabRefreshed(TSharedPtr<SDockTab> Tab) const
{
    TSharedRef<STimelineEditor> TimelineEditor = StaticCastSharedRef<STimelineEditor>(Tab->GetContent());
    TimelineEditor->OnTimelineChanged();
}

FTimelineEditorSummoner::FTimelineEditorSummoner(TSharedPtr<class FOdysseyBrushEditor> InOdysseyBrushEditorPtr)
    : FDocumentTabFactoryForObjects<UTimelineTemplate>(FOdysseyBrushEditorTabs::TimelineEditorID, InOdysseyBrushEditorPtr)
, OdysseyBrushEditorPtr(InOdysseyBrushEditorPtr)
{

}

TSharedRef<SWidget> FTimelineEditorSummoner::CreateTabBodyForObject(const FWorkflowTabSpawnInfo& Info, UTimelineTemplate* DocumentID) const
{
    return SNew(STimelineEditor, OdysseyBrushEditorPtr.Pin(), DocumentID);
}

const FSlateBrush* FTimelineEditorSummoner::GetTabIconForObject(const FWorkflowTabSpawnInfo& Info, UTimelineTemplate* DocumentID) const
{
    return FEditorStyle::GetBrush("GraphEditor.Timeline_16x");
}

void FTimelineEditorSummoner::SaveState(TSharedPtr<SDockTab> Tab, TSharedPtr<FTabPayload> Payload) const
{
    UTimelineTemplate* Timeline = FTabPayload_UObject::CastChecked<UTimelineTemplate>(Payload);
    OdysseyBrushEditorPtr.Pin()->GetBlueprintObj()->LastEditedDocuments.Add(FEditedDocumentInfo(Timeline));
}

TAttribute<FText> FTimelineEditorSummoner::ConstructTabNameForObject(UTimelineTemplate* DocumentID) const
{
    return TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateStatic<UObject*>(&FLocalKismetCallbacks::GetObjectName, DocumentID));
}

FDebugInfoSummoner::FDebugInfoSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp) : FWorkflowTabFactory(FOdysseyBrushEditorTabs::DebugID, InHostingApp)
{
    TabLabel = LOCTEXT("DebugTabTitle", "Debug");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "DebugTools.TabIcon");

    EnableTabPadding();
    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("DebugView", "Debug");
    ViewMenuTooltip = LOCTEXT("DebugView_ToolTip", "Shows the debugging view");
}

TSharedRef<SWidget> FDebugInfoSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetDebuggingView();
}

FDefaultsEditorSummoner::FDefaultsEditorSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::DefaultEditorID, InHostingApp)
{
    TabLabel = LOCTEXT("ClassDefaultsTabTitle", "Class Defaults");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.OdysseyBrushDefaults");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("DefaultEditorView", "Defaults");
    ViewMenuTooltip = LOCTEXT("DefaultEditorView_ToolTip", "Shows the default editor view");
}

TSharedRef<SWidget> FDefaultsEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    TSharedRef<SWidget> Message = CreateOptionalDataOnlyMessage();

    return SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(FMargin(0,0,0,1))
        [
            Message
        ]

        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            OdysseyBrushEditorPtr->GetDefaultEditor()
        ];
}

TSharedRef<SWidget> FDefaultsEditorSummoner::CreateOptionalDataOnlyMessage() const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    TSharedRef<SWidget> Message = SNullWidget::NullWidget;
    if ( UBlueprint* OdysseyBrush = OdysseyBrushEditorPtr->GetBlueprintObj() )
    {
        if ( FBlueprintEditorUtils::IsDataOnlyBlueprint(OdysseyBrush) )
        {
            Message = SNew(SBorder)
                .Padding(FMargin(5))
                .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
                [
                    SNew(SWrapBox)
                    .UseAllottedWidth(true)

                    + SWrapBox::Slot()
                    [
                        SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("BoldFont"))
                        .Text(LOCTEXT("DataOnlyMessage_Part1", "NOTE: This is a data only blueprint, so only the default values are shown.  It does not have any script or variables.  If you want to add some, "))
                    ]

                    + SWrapBox::Slot()
                    [
                        SNew(SHyperlink)
                        .Style(FEditorStyle::Get(), "Common.GotoOdysseyBrushHyperlink")
                        .OnNavigate(this, &FDefaultsEditorSummoner::OnChangeOdysseyBrushToNotDataOnly)
                        .Text(LOCTEXT("FullEditor", "Open Full OdysseyBrush Editor"))
                        .ToolTipText(LOCTEXT("FullEditorToolTip", "This opens the blueprint in the full editor."))
                    ]
                ];
        }
    }

    return Message;
}

void FDefaultsEditorSummoner::OnChangeOdysseyBrushToNotDataOnly()
{
    UBlueprint* OdysseyBrush = nullptr;

    {
        TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
        OdysseyBrush = OdysseyBrushEditorPtr->GetBlueprintObj();
        if ( OdysseyBrush )
        {
            OdysseyBrushEditorPtr->CloseWindow();
        }
    }

    if ( OdysseyBrush )
    {
        OdysseyBrush->bForceFullEditor = true;

        GEditor->EditObject(OdysseyBrush);
    }
}

FConstructionScriptEditorSummoner::FConstructionScriptEditorSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::ConstructionScriptEditorID, InHostingApp)
{
    TabLabel = LOCTEXT("ComponentsTabLabel", "Components");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Components");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("ComponentsView", "Components");
    ViewMenuTooltip = LOCTEXT("ComponentsView_ToolTip", "Show the components view");
}

TSharedRef<SWidget> FConstructionScriptEditorSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetSCSEditor().ToSharedRef();
}

FSCSViewportSummoner::FSCSViewportSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::SCSViewportID, InHostingApp)
{
    TabLabel = LOCTEXT("SCSViewportTabLabel", "Viewport");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports");

    bIsSingleton = true;
    TabRole = ETabRole::DocumentTab;

    ViewMenuDescription = LOCTEXT("SCSViewportView", "Viewport");
    ViewMenuTooltip = LOCTEXT("SCSViewportView_ToolTip", "Show the viewport view");
}

TSharedRef<SWidget> FSCSViewportSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    TSharedPtr<SWidget> Result;
    if (OdysseyBrushEditorPtr->CanAccessComponentsMode())
    {
        Result = OdysseyBrushEditorPtr->GetSCSViewport();
    }

    if (Result.IsValid())
    {
        return Result.ToSharedRef();
    }
    else
    {
        return SNew(SErrorText)
            .BackgroundColor(FLinearColor::Transparent)
            .ErrorText(LOCTEXT("SCSViewportView_Unavailable", "Viewport is not available for this OdysseyBrush."));
    }
}

TSharedRef<SDockTab> FSCSViewportSummoner::SpawnTab(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedRef<SDockTab> Tab = FWorkflowTabFactory::SpawnTab(Info);

    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
    OdysseyBrushEditorPtr->GetSCSViewport()->SetOwnerTab(Tab);

    return Tab;
}

FPaletteSummoner::FPaletteSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::PaletteID, InHostingApp)
{
    TabLabel = LOCTEXT("PaletteTabTitle", "Palette");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("PaletteView", "Palette");
    ViewMenuTooltip = LOCTEXT("PaletteView_ToolTip", "Show palette of all functions and variables");
}

TSharedRef<SWidget> FPaletteSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetPalette();
}

FBookmarksSummoner::FBookmarksSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::BookmarksID, InHostingApp)
{
    TabLabel = LOCTEXT("BookmarksTabTitle", "Bookmarks");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Bookmarks");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("BookmarksView", "Bookmarks");
    ViewMenuTooltip = LOCTEXT("BookmarksView_ToolTip", "Show bookmarks associated with this OdysseyBrush");
}

TSharedRef<SWidget> FBookmarksSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetBookmarksWidget();
}

FMyOdysseyBrushSummoner::FMyOdysseyBrushSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::MyOdysseyBrushID, InHostingApp)
{
    TabLabel = LOCTEXT("MyOdysseyBrushTabLabel", "My OdysseyBrush");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.OdysseyBrushCore");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("MyOdysseyBrushTabView", "My OdysseyBrush");
    ViewMenuTooltip = LOCTEXT("MyOdysseyBrushTabView_ToolTip", "Show the my blueprint view");
}

TSharedRef<SWidget> FMyOdysseyBrushSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetMyOdysseyBrushWidget().ToSharedRef();
}

FReplaceNodeReferencesSummoner::FReplaceNodeReferencesSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::ReplaceNodeReferencesID, InHostingApp)
{
    TabLabel = LOCTEXT("ReplaceNodeReferences", "Replace References");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "ClassIcon.OdysseyBrushCore");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("ReplaceNodeReferences", "Replace References");
    ViewMenuTooltip = LOCTEXT("ReplaceNodeReferences_Tooltip", "Show Replace References");
}

TSharedRef<SWidget> FReplaceNodeReferencesSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetReplaceReferencesWidget().ToSharedRef();
}

FCompilerResultsSummoner::FCompilerResultsSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::CompilerResultsID, InHostingApp)
{
    TabLabel = LOCTEXT("CompilerResultsTabTitle", "Compiler Results");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.CompilerResults");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("CompilerResultsView", "Compiler Results");
    ViewMenuTooltip = LOCTEXT("CompilerResultsView_ToolTip", "Show compiler results of all functions and variables");
}

TSharedRef<SWidget> FCompilerResultsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetCompilerResults();
}

FFindResultsSummoner::FFindResultsSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::FindResultsID, InHostingApp)
{
    TabLabel = LOCTEXT("FindResultsTabTitle", "Find Results");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.FindResults");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("FindResultsView", "Find Results");

    if (GetDefault<UBlueprintEditorSettings>()->bHostFindInBlueprintsInGlobalTab)
    {
        ViewMenuTooltip = LOCTEXT("FindResultsView_ToolTip", "Show find results for searching in this blueprint");
    }
    else
    {
        ViewMenuTooltip = LOCTEXT("FindResultsViewAllBlueprints_ToolTip", "Show find results for searching in this blueprint or all blueprints");
    }
}

TSharedRef<SWidget> FFindResultsSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());

    return OdysseyBrushEditorPtr->GetFindResults();
}


//


FOdysseyBrushPreviewSummoner::FOdysseyBrushPreviewSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::PreviewID, InHostingApp)
{
    TabLabel = LOCTEXT("OdysseyBrushPreviewTabTitle", "Preview");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette");
    bIsSingleton = true;
    this->bShouldAutosize = true;

    ViewMenuDescription = LOCTEXT("OdysseyBrushPreviewView", "Preview Brush");
    ViewMenuTooltip = LOCTEXT("OdysseyBrushPreviewView_ToolTip", "Show Brush Preview");
}

TSharedRef<SWidget> FOdysseyBrushPreviewSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
    return OdysseyBrushEditorPtr->GetOdysseyBrushPreview();
}

//
//


FOdysseyBrushTestAreaSummoner::FOdysseyBrushTestAreaSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::TestAreaID, InHostingApp)
{
    TabLabel = LOCTEXT("OdysseyBrushTestAreaTabTitle", "TestArea");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("OdysseyBrushTestAreaView", "Brush Test Area");
    ViewMenuTooltip = LOCTEXT("OdysseyBrushTestAreaView_ToolTip", "Show Brush TestArea");
}

TSharedRef<SWidget> FOdysseyBrushTestAreaSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
    return OdysseyBrushEditorPtr->GetOdysseyBrushTestArea();
}


//
//


FOdysseyBrushPreferencesOverridesSummoner::FOdysseyBrushPreferencesOverridesSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::PreferencesOverridesID, InHostingApp)
{
    TabLabel = LOCTEXT("OdysseyBrushPreferencesOverridesTabTitle", "Overrides");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("OdysseyBrushPreferencesOverridesView", "Brush Preferences Overrides");
    ViewMenuTooltip = LOCTEXT("OdysseyBrushPreferencesOverridesView_ToolTip", "Show Brush Preferences Overrides Tab");
}

TSharedRef<SWidget> FOdysseyBrushPreferencesOverridesSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
    return OdysseyBrushEditorPtr->GetOdysseyBrushPreferencesOverrides();
}


//
//


FOdysseyBrushInternalExposedSummoner::FOdysseyBrushInternalExposedSummoner(TSharedPtr<class FAssetEditorToolkit> InHostingApp)
    : FWorkflowTabFactory(FOdysseyBrushEditorTabs::ExposedID, InHostingApp)
{
    TabLabel = LOCTEXT("OdysseyBrushInternalExposedTabTitle", "Exposed");
    TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette");

    bIsSingleton = true;

    ViewMenuDescription = LOCTEXT("OdysseyBrushInternalExposedView", "Brush Exposed Parameters");
    ViewMenuTooltip = LOCTEXT("OdysseyBrushInternalExposedView_ToolTip", "Show Brush Exposed Parameters Tab");
}

TSharedRef<SWidget> FOdysseyBrushInternalExposedSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = StaticCastSharedPtr<FOdysseyBrushEditor>(HostingApp.Pin());
    return OdysseyBrushEditorPtr->GetOdysseyBrushInternalExposed();
}

//


void FGraphTabHistory::EvokeHistory(TSharedPtr<FTabInfo> InTabInfo, bool bPrevTabMatches)
{
    FWorkflowTabSpawnInfo SpawnInfo;
    SpawnInfo.Payload = Payload;
    SpawnInfo.TabInfo = InTabInfo;

    if(bPrevTabMatches)
    {
        TSharedPtr<SDockTab> DockTab = InTabInfo->GetTab().Pin();
        GraphEditor = StaticCastSharedRef<SGraphEditor>(DockTab->GetContent());
    }
    else
    {
        TSharedRef< SGraphEditor > GraphEditorRef = StaticCastSharedRef< SGraphEditor >(FactoryPtr.Pin()->CreateTabBody(SpawnInfo));
        GraphEditor = GraphEditorRef;
        FactoryPtr.Pin()->UpdateTab(InTabInfo->GetTab().Pin(), SpawnInfo, GraphEditorRef);
    }
}

void FGraphTabHistory::SaveHistory()
{
    if (IsHistoryValid())
    {
        check(GraphEditor.IsValid());
        GraphEditor.Pin()->GetViewLocation(SavedLocation, SavedZoomAmount);
        GraphEditor.Pin()->GetViewBookmark(SavedBookmarkId);
    }
}

void FGraphTabHistory::RestoreHistory()
{
    if (IsHistoryValid())
    {
        check(GraphEditor.IsValid());
        GraphEditor.Pin()->SetViewLocation(SavedLocation, SavedZoomAmount, SavedBookmarkId);
    }
}


#undef LOCTEXT_NAMESPACE
