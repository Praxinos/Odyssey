// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
    void RegisterCommands();
    void UnregisterCommands();

    void RegisterTrackEditors();
    void UnregisterTrackEditors();

private:
    FDelegateHandle CinematicBoardTrackCreateEditorHandle;
    FDelegateHandle SingleCameraCutTrackCreateEditorHandle;
    FDelegateHandle NoteTrackCreateEditorHandle;
};
