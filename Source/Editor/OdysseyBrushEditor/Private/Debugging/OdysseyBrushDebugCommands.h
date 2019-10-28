// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

//////////////////////////////////////////////////////////////////////////
// FDebuggingActionCallbacks

class FDebuggingActionCallbacks
{
public:
    static void ClearWatches(class UBlueprint* OdysseyBrush);
    static void ClearWatch(class UEdGraphPin* WatchedPin);
    static void ClearBreakpoints(class UBlueprint* OwnerOdysseyBrush);
    static void ClearBreakpoint(class UBreakpoint* Breakpoint, class UBlueprint* OwnerOdysseyBrush);
    static void SetBreakpointEnabled(class UBreakpoint* Breakpoint, bool bEnabled);
    static void SetEnabledOnAllBreakpoints(class UBlueprint* OwnerOdysseyBrush, bool bShouldBeEnabled);
};

//////////////////////////////////////////////////////////////////////////
