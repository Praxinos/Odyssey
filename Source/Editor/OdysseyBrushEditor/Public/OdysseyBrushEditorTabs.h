// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct ODYSSEYBRUSHEDITOR_API FOdysseyBrushEditorTabs
{
    // Tab identifiers
    static const FName DetailsID;
    static const FName DefaultEditorID;
    static const FName DebugID;
    static const FName PaletteID;
    static const FName BookmarksID;
    static const FName CompilerResultsID;
    static const FName FindResultsID;
    static const FName ConstructionScriptEditorID;
    static const FName SCSViewportID;
    static const FName MyOdysseyBrushID;
    static const FName ReplaceNodeReferencesID;
    static const FName UserDefinedStructureID;

    // Brush tab identifiers
    static const FName PreviewID;
    static const FName ExposedID;
    static const FName PreferencesOverridesID;
    static const FName TestAreaID;

    // Document tab identifiers
    static const FName GraphEditorID;
    static const FName TimelineEditorID;

private:
    FOdysseyBrushEditorTabs() {}
};

