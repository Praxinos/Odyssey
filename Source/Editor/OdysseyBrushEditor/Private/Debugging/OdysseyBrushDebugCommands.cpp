// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000


#include "Debugging/OdysseyBrushDebugCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetDebugUtilities.h"

//////////////////////////////////////////////////////////////////////////
// FDebuggingActionCallbacks

void FDebuggingActionCallbacks::ClearWatches(UBlueprint* OdysseyBrush)
{
    FKismetDebugUtilities::ClearPinWatches(OdysseyBrush);
}

void FDebuggingActionCallbacks::ClearWatch(UEdGraphPin* WatchedPin)
{
    UBlueprint* OdysseyBrush = FBlueprintEditorUtils::FindBlueprintForNode(WatchedPin->GetOwningNode());
    if (OdysseyBrush != NULL)
    {
        FKismetDebugUtilities::RemovePinWatch(OdysseyBrush, WatchedPin);
    }
}

void FDebuggingActionCallbacks::ClearBreakpoints(UBlueprint* OwnerOdysseyBrush)
{
    FKismetDebugUtilities::ClearBreakpoints(OwnerOdysseyBrush);
}

void FDebuggingActionCallbacks::ClearBreakpoint(UBreakpoint* Breakpoint, UBlueprint* OwnerOdysseyBrush)
{
    FKismetDebugUtilities::StartDeletingBreakpoint(Breakpoint, OwnerOdysseyBrush);
}

void FDebuggingActionCallbacks::SetBreakpointEnabled(UBreakpoint* Breakpoint, bool bEnabled)
{
    FKismetDebugUtilities::SetBreakpointEnabled(Breakpoint, bEnabled);
}

void FDebuggingActionCallbacks::SetEnabledOnAllBreakpoints(UBlueprint* OwnerOdysseyBrush, bool bShouldBeEnabled)
{
    for (TArray<UBreakpoint*>::TIterator BreakpointIt(OwnerOdysseyBrush->Breakpoints); BreakpointIt; ++BreakpointIt)
    {
        UBreakpoint* BP = *BreakpointIt;
        FKismetDebugUtilities::SetBreakpointEnabled(BP, bShouldBeEnabled);
    }
}
