// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "Templates/UniquePtr.h"

struct FOdysseyBrushDebugger
{
    // Initializes the global state of the debugger (commands, tab spawners, etc):
    FOdysseyBrushDebugger();

    // Destructor declaration purely so that we can pimpl:
    ~FOdysseyBrushDebugger();

private:
    TUniquePtr< struct FOdysseyBrushDebuggerImpl > Impl;

    // prevent copying:
    FOdysseyBrushDebugger(const FOdysseyBrushDebugger&);
    FOdysseyBrushDebugger(FOdysseyBrushDebugger&&);
    FOdysseyBrushDebugger& operator=(FOdysseyBrushDebugger const&);
    FOdysseyBrushDebugger& operator=(FOdysseyBrushDebugger&&);
};

