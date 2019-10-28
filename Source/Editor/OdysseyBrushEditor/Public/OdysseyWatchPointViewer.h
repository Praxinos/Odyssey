// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

struct FAssetData;
class UBlueprint;

namespace WatchViewer
{
    // updates the instanced watch values, these are only valid while execution is paused
    void ODYSSEYBRUSHEDITOR_API UpdateInstancedWatchDisplay();

    // called when we unpause execution and set watch values back to the blueprint versions
    void ODYSSEYBRUSHEDITOR_API ContinueExecution();

    // called when we are adding or changing watches from OdysseyBrushObj
    void ODYSSEYBRUSHEDITOR_API UpdateWatchListFromOdysseyBrush(TWeakObjectPtr<UBlueprint> OdysseyBrushObj);

    // called when we want to remove watches in the watch window from a blueprint
    // does NOT remove watches from the pins in the blueprint object
    void ODYSSEYBRUSHEDITOR_API RemoveWatchesForOdysseyBrush(TWeakObjectPtr<UBlueprint> OdysseyBrushObj);

    // called when we want to remove watches in the watch window from a blueprint
    // does NOT remove watches from the pins in the blueprint object
    void ODYSSEYBRUSHEDITOR_API RemoveWatchesForAsset(const FAssetData& AssetData);

    // called when an asset is renamed; updates the watches on the asset
    void ODYSSEYBRUSHEDITOR_API OnRenameAsset(const FAssetData& AssetData, const FString& OldAssetName);

    // called when a OdysseyBrushObj should no longer be watched
    void ODYSSEYBRUSHEDITOR_API ClearWatchListFromOdysseyBrush(TWeakObjectPtr<UBlueprint> OdysseyBrushObj);

    FName GetTabName();
    void RegisterTabSpawner(FTabManager& TabManager);
}
