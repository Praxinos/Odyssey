// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FNodeSpawnInfo;
class UEdGraph;
class UEdGraphNode;

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushEditorCommands

/** Set of kismet 2 wide commands */
class FOdysseyBrushEditorCommands : public TCommands<FOdysseyBrushEditorCommands>
{
public:

    FOdysseyBrushEditorCommands()
        : TCommands<FOdysseyBrushEditorCommands>( TEXT("OdysseyBrushEditor"), NSLOCTEXT("Contexts", "OdysseyBrushEditor", "OdysseyBrush Editor"), NAME_None, FEditorStyle::GetStyleSetName() )
    {
    }

    virtual void RegisterCommands() override;


    // File-ish commands
    TSharedPtr< FUICommandInfo > CompileBlueprint;
    TSharedPtr< FUICommandInfo > RefreshAllNodes;
    TSharedPtr< FUICommandInfo > DeleteUnusedVariables;

    TSharedPtr< FUICommandInfo > FindReferencesFromClass;
    TSharedPtr< FUICommandInfo > FindReferencesFromOdysseyBrush;
    TSharedPtr< FUICommandInfo > RepairCorruptedOdysseyBrush;

    // Edit commands
    TSharedPtr< FUICommandInfo > FindInOdysseyBrush;
    TSharedPtr< FUICommandInfo > ReparentOdysseyBrush;

    // View commands
    TSharedPtr< FUICommandInfo > ZoomToWindow;
    TSharedPtr< FUICommandInfo > ZoomToSelection;
    TSharedPtr< FUICommandInfo > NavigateToParent;
    TSharedPtr< FUICommandInfo > NavigateToParentBackspace;
    TSharedPtr< FUICommandInfo > NavigateToChild;

    // Preview commands
    TSharedPtr< FUICommandInfo > ResetCamera;
    TSharedPtr< FUICommandInfo > EnableSimulation;
    TSharedPtr< FUICommandInfo > ShowFloor;
    TSharedPtr< FUICommandInfo > ShowGrid;

    // Debugging commands
    TSharedPtr< FUICommandInfo > EnableAllBreakpoints;
    TSharedPtr< FUICommandInfo > DisableAllBreakpoints;
    TSharedPtr< FUICommandInfo > ClearAllBreakpoints;
    TSharedPtr< FUICommandInfo > ClearAllWatches;

    // New documents
    TSharedPtr< FUICommandInfo > AddNewVariable;
    TSharedPtr< FUICommandInfo > AddNewLocalVariable;
    TSharedPtr< FUICommandInfo > AddNewFunction;
    TSharedPtr< FUICommandInfo > AddNewMacroDeclaration;
    TSharedPtr< FUICommandInfo > AddNewAnimationGraph;
    TSharedPtr< FUICommandInfo > AddNewEventGraph;
    TSharedPtr< FUICommandInfo > AddNewDelegate;

    // Development commands
    TSharedPtr< FUICommandInfo > SaveIntermediateBuildProducts;
    TSharedPtr< FUICommandInfo > GenerateNativeCode;
    TSharedPtr< FUICommandInfo > ShowActionMenuItemSignatures;

    // SSC commands
    TSharedPtr< FUICommandInfo > BeginOdysseyBrushMerge;
};

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushSpawnNodeCommands

/** Handles spawn node commands for the OdysseyBrush Editor */
class FOdysseyBrushSpawnNodeCommands : public TCommands<FOdysseyBrushSpawnNodeCommands>
{
public:

    FOdysseyBrushSpawnNodeCommands()
        : TCommands<FOdysseyBrushSpawnNodeCommands>(TEXT("OdysseyBrushEditorSpawnNodes"), NSLOCTEXT("Contexts", "OdysseyBrushEditor_SpawnNodes", "OdysseyBrush Editor - Spawn Nodes by chord"), NAME_None, FEditorStyle::GetStyleSetName())
    {
    }

    virtual void RegisterCommands() override;

    /**
     * Returns a graph action assigned to the passed in chord
     *
     * @param InChord                    The chord to use for lookup
     * @param InDestGraph                The graph to create the graph action for, used for validation purposes and to link any important node data to the graph
     * @param InOutDestPosition            Position to start placing nodes, will be updated to be at the next safe position for node placement
     * @param OutNodes                    All nodes spawned by this operation
     */
    void GetGraphActionByChord(FInputChord& InChord, UEdGraph* InDestGraph, FVector2D& InOutDestPosition, TArray<UEdGraphNode*>& OutNodes) const;

private:
    /** An array of all the possible commands for spawning nodes */
    TArray< TSharedPtr< class FNodeSpawnInfo > > NodeCommands;
};

//////////////////////////////////////////////////////////////////////////
// FSCSEditorViewportCommands

class FSCSEditorViewportCommands : public TCommands<FSCSEditorViewportCommands>
{
public:

    FSCSEditorViewportCommands()
        : TCommands<FSCSEditorViewportCommands>(TEXT("SCSEditorViewport"), NSLOCTEXT("Contexts", "SCSEditorViewport", "SCS Editor Viewport"), NAME_None, FEditorStyle::GetStyleSetName())
    {}

    virtual void RegisterCommands() override;

    TSharedPtr< FUICommandInfo > DeleteComponent;
};
