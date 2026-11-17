// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

/** Commands specific to animation layers. */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationLayerStackEditorCommands : public TCommands<FOdysseyAnimationLayerStackEditorCommands>
{
public:
    FOdysseyAnimationLayerStackEditorCommands();
    virtual void RegisterCommands() override;

    TSharedPtr<FUICommandInfo> ActivateLayerLighttable;
    TSharedPtr<FUICommandInfo> InactivateLayerLighttable;
    TSharedPtr<FUICommandInfo> ToggleLayerLighttable;
};
