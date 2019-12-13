// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Widgets/SWidget.h"
#include "Textures/SlateIcon.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"
#include "WorkflowOrientedApp/SModeWidget.h"

class FOdysseyBrushEditor;
class FExtender;
class FMenuBuilder;
class FToolBarBuilder;

/**
 * Kismet menu
 */
class ODYSSEYBRUSHEDITOR_API FKismet2Menu
{
public:
    static void SetupOdysseyBrushEditorMenu( TSharedPtr< FExtender > Extender, FOdysseyBrushEditor& Kismet);

protected:
    static void FillFileMenUBlueprintSection( FMenuBuilder& MenuBuilder, FOdysseyBrushEditor& Kismet );

    static void FillEditMenu( FMenuBuilder& MenuBuilder );

    static void FillViewMenu( FMenuBuilder& MenuBuilder );

    static void FillDebugMenu( FMenuBuilder& MenuBuilder );

    static void FillDeveloperMenu( FMenuBuilder& MenuBuilder );

private:
    /** Diff current blueprint against the specified revision */
    static void DiffAgainstRevision( class UBlueprint* Current, int32 OldRevision );

    static TSharedRef<SWidget> MakeDiffMenu(FOdysseyBrushEditor& Kismet);
};


class FFullOdysseyBrushEditorCommands : public TCommands<FFullOdysseyBrushEditorCommands>
{
public:
    /** Constructor */
    FFullOdysseyBrushEditorCommands()
        : TCommands<FFullOdysseyBrushEditorCommands>("FullOdysseyBrushEditor", NSLOCTEXT("Contexts", "FullOdysseyBrushEditor", "Full OdysseyBrush Editor"), NAME_None, FEditorStyle::GetStyleSetName())
    {
    }

    /** Compile the blueprint */
    TSharedPtr<FUICommandInfo> Compile;
    TSharedPtr<FUICommandInfo> SaveOnCompile_Never;
    TSharedPtr<FUICommandInfo> SaveOnCompile_SuccessOnly;
    TSharedPtr<FUICommandInfo> SaveOnCompile_Always;
    TSharedPtr<FUICommandInfo> JumpToErrorNode;

    /** Switch between modes in the blueprint editor */
    TSharedPtr<FUICommandInfo> SwitchToScriptingMode;
    TSharedPtr<FUICommandInfo> SwitchToOdysseyBrushDefaultsMode;
    TSharedPtr<FUICommandInfo> SwitchToComponentsMode;

    /** Edit OdysseyBrush global options */
    TSharedPtr<FUICommandInfo> EditGlobalOptions;
    TSharedPtr<FUICommandInfo> EditClassDefaults;

    /** Initialize commands */
    virtual void RegisterCommands() override;
};



class ODYSSEYBRUSHEDITOR_API FOdysseyBrushEditorToolbar : public TSharedFromThis<FOdysseyBrushEditorToolbar>
{
public:
    FOdysseyBrushEditorToolbar(TSharedPtr<FOdysseyBrushEditor> InOdysseyBrushEditor)
        : OdysseyBrushEditor(InOdysseyBrushEditor) {}

    void AddOdysseyBrushEditorModesToolbar(TSharedPtr<FExtender> Extender);
    void AddOdysseyBrushGlobalOptionsToolbar(TSharedPtr<FExtender> Extender);
    void AddCompileToolbar(TSharedPtr<FExtender> Extender);
    void AddNewToolbar(TSharedPtr<FExtender> Extender);
    void AddScriptingToolbar(TSharedPtr<FExtender> Extender);
    void AddDebuggingToolbar(TSharedPtr<FExtender> Extender);
    void AddComponentsToolbar(TSharedPtr<FExtender> Extender);

    /** Returns the current status icon for the blueprint being edited */
    FSlateIcon GetStatusImage() const;

    /** Returns the current status as text for the blueprint being edited */
    FText GetStatusTooltip() const;

    /** Helper function for generating the buttons in the toolbar, reused by merge and diff tools */
    static TArray< TSharedPtr< class SWidget> > GenerateToolbarWidgets(const class UBlueprint* OdysseyBrushObj, TAttribute<FName> ActiveModeGetter, FOnModeChangeRequested ActiveModeSetter);

private:
    void FillOdysseyBrushEditorModesToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillOdysseyBrushGlobalOptionsToolbar(FToolBarBuilder& ToolBarBuilder);
    void FillCompileToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillNewToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillScriptingToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillDebuggingToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillComponentsToolbar(FToolBarBuilder& ToolbarBuilder);

protected:
    /** Pointer back to the blueprint editor tool that owns us */
    TWeakPtr<FOdysseyBrushEditor> OdysseyBrushEditor;
};

