// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEposTracksEditorModule : public IModuleInterface
{
public:

    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    FDelegateHandle CinematicBoardTrackCreateEditorHandle;
    FDelegateHandle AudioTrackCreateEditorHandle;
    FDelegateHandle LevelVisibilityTrackCreateEditorHandle;
};
