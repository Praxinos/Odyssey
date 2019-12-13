// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushEditorModes.h"
#include "Settings/EditorExperimentalSettings.h"


// Core kismet tabs
#include "SOdysseySCSEditor.h"
#include "SOdysseySCSEditorViewport.h"
#include "SOdysseyInspector.h"
#include "SMyOdysseyBrush.h"
#include "SOdysseyBrushPreferencesOverrides.h"
// End of core kismet tabs

// Debugging
// End of debugging

#include "Framework/Docking/LayoutExtender.h"
#include "SOdysseyBrushEditorToolbar.h"
#include "OdysseyBrushEditorTabs.h"
#include "OdysseyBrushEditorTabFactories.h"
#include "OdysseyBrushEditorSharedTabFactories.h"
#include "BlueprintEditorSettings.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorModes"


const FName FOdysseyBrushEditorApplicationModes::StandardOdysseyBrushEditorMode( TEXT("OdysseyBrushGraphName") );

////////////////////////////////////////
//
FOdysseyBrushEditorUnifiedMode::FOdysseyBrushEditorUnifiedMode(TSharedPtr<class FOdysseyBrushEditor> InOdysseyBrushEditor, FName InModeName, FText(*GetLocalizedMode)( const FName ), const bool bRegisterViewport)
    : FApplicationMode(InModeName, GetLocalizedMode)
{
    MyOdysseyBrushEditor = InOdysseyBrushEditor;

    // Create the tab factories
    //OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FDebugInfoSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FPaletteSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FMyOdysseyBrushSummoner(InOdysseyBrushEditor)));
    if (GetDefault<UEditorExperimentalSettings>()->bEnableFindAndReplaceReferences)
    {
        OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FReplaceNodeReferencesSummoner(InOdysseyBrushEditor)));
    }
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FCompilerResultsSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FFindResultsSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FOdysseyBrushPreferencesOverridesSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FOdysseyBrushPreviewSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FOdysseyBrushTestAreaSummoner(InOdysseyBrushEditor)));
    OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FOdysseyBrushInternalExposedSummoner(InOdysseyBrushEditor)));
    //OdysseyBrushEditorTabFactories.RegisterFactory(MakeShareable(new FBookmarksSummoner(InOdysseyBrushEditor)));

    CoreTabFactories.RegisterFactory(MakeShareable(new FOdysseySelectionDetailsSummoner(InOdysseyBrushEditor)));

    TabLayout = FTabManager::NewLayout( "OdysseyBrush_Unified_v2" )
    ->AddArea
    (
    FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
    ->Split
        (
            FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
                ->SetSizeCoefficient(0.15f)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.1f)
                    ->SetHideTabWell(true)
                    ->AddTab( FOdysseyBrushEditorTabs::PreviewID, ETabState::OpenedTab )
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.5f)
                    ->AddTab( FOdysseyBrushEditorTabs::MyOdysseyBrushID, ETabState::OpenedTab )
                )
                /*
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.4f)
                    ->AddTab( FOdysseyBrushEditorTabs::PreferencesOverridesID, ETabState::OpenedTab )
                )
                */
            )
            ->Split
            (
                FTabManager::NewSplitter()->SetOrientation( Orient_Vertical )
                ->SetSizeCoefficient(0.60f)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.186721f)
                    ->SetHideTabWell(true)
                    ->AddTab(InOdysseyBrushEditor->GetToolbarTabId(), ETabState::OpenedTab)
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient( 0.80f )
                    ->AddTab( "Document", ETabState::ClosedTab )
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient( 0.20f )
                    ->AddTab( FOdysseyBrushEditorTabs::CompilerResultsID, ETabState::ClosedTab )
                    ->AddTab( FOdysseyBrushEditorTabs::FindResultsID, ETabState::ClosedTab )
                )
            )
            ->Split
            (
                FTabManager::NewSplitter() ->SetOrientation( Orient_Vertical )
                ->SetSizeCoefficient(0.25f)
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.2f)
                    ->AddTab( FOdysseyBrushEditorTabs::ExposedID, ETabState::OpenedTab )
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.2f)
                    ->AddTab( FOdysseyBrushEditorTabs::PaletteID, ETabState::OpenedTab )
                    ->AddTab( FOdysseyBrushEditorTabs::DetailsID, ETabState::OpenedTab )
                )
                ->Split
                (
                    FTabManager::NewStack()
                    ->SetSizeCoefficient(0.6f)
                    ->AddTab( FOdysseyBrushEditorTabs::TestAreaID, ETabState::OpenedTab )
                )
            )
        )
    );

    // setup toolbar
    //@TODO: Keep this in sync with AnimBlueprintMode.cpp
    //InOdysseyBrushEditor->GetToolbarBuilder()->AddOdysseyBrushEditorModesToolbar(ToolbarExtender);
    InOdysseyBrushEditor->GetToolbarBuilder()->AddCompileToolbar(ToolbarExtender);
    InOdysseyBrushEditor->GetToolbarBuilder()->AddScriptingToolbar(ToolbarExtender);
    //InOdysseyBrushEditor->GetToolbarBuilder()->AddOdysseyBrushGlobalOptionsToolbar(ToolbarExtender);

    /*
    if ( bRegisterViewport )
    {
        InOdysseyBrushEditor->GetToolbarBuilder()->AddComponentsToolbar(ToolbarExtender);
    }
    */

    //InOdysseyBrushEditor->GetToolbarBuilder()->AddDebuggingToolbar(ToolbarExtender);

    FOdysseyBrushEditorModule& OdysseyBrushEditorModule = FModuleManager::LoadModuleChecked<FOdysseyBrushEditorModule>("OdysseyBrushEditor");
    OdysseyBrushEditorModule.OnRegisterTabsForEditor().Broadcast(OdysseyBrushEditorTabFactories, InModeName, InOdysseyBrushEditor);

    LayoutExtender = MakeShared<FLayoutExtender>();
    OdysseyBrushEditorModule.OnRegisterLayoutExtensions().Broadcast(*LayoutExtender);
}

void FOdysseyBrushEditorUnifiedMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
    TSharedPtr<FOdysseyBrushEditor> BP = MyOdysseyBrushEditor.Pin();

    BP->RegisterToolbarTab(InTabManager.ToSharedRef());

    // Mode-specific setup
    BP->PushTabFactories(CoreTabFactories);
    BP->PushTabFactories(OdysseyBrushEditorOnlyTabFactories);
    BP->PushTabFactories(OdysseyBrushEditorTabFactories);

    FApplicationMode::RegisterTabFactories(InTabManager);
}

void FOdysseyBrushEditorUnifiedMode::PreDeactivateMode()
{
    FApplicationMode::PreDeactivateMode();

    TSharedPtr<FOdysseyBrushEditor> BP = MyOdysseyBrushEditor.Pin();

    BP->SaveEditedObjectState();
    BP->GetMyOdysseyBrushWidget()->ClearGraphActionMenuSelection();
}

void FOdysseyBrushEditorUnifiedMode::PostActivateMode()
{
    // Reopen any documents that were open when the blueprint was last saved
    TSharedPtr<FOdysseyBrushEditor> BP = MyOdysseyBrushEditor.Pin();
    BP->RestoreEditedObjectState();
    BP->SetupViewForOdysseyBrushEditingMode();

    FApplicationMode::PostActivateMode();
}

#undef LOCTEXT_NAMESPACE
