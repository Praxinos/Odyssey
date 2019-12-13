// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Widgets/SCompoundWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class FTabManager;
struct FFrame;

namespace CallStackViewer
{
    void ODYSSEYBRUSHEDITOR_API UpdateDisplayedCallstack(const TArray<const FFrame*>& ScriptStack);
    FName GetTabName();
    void RegisterTabSpawner(FTabManager& TabManager);
}
