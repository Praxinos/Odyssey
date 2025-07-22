// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Internationalization/Internationalization.h"
#include "Styling/AppStyle.h"
#include "Templates/SharedPointer.h"
#include "UObject/NameTypes.h"
#include "UObject/UnrealNames.h"

class FUICommandInfo;

class FStoryboardViewportCommands : public TCommands<FStoryboardViewportCommands>
{
public:
    FStoryboardViewportCommands(): TCommands<FStoryboardViewportCommands>
    (
        "Storyboard Viewport",
        NSLOCTEXT( "Contexts", "StoryboardViewport", "Storyboard Viewport" ),
        NAME_None,
        FAppStyle::GetAppStyleSetName() // Icon Style Set
    )
    {
    }

    /** Disabled */
    TSharedPtr<FUICommandInfo> Disabled;

    /** Grid 2x2 */
    TSharedPtr<FUICommandInfo> Grid2x2;

    /** Grid 3x3 */
    TSharedPtr<FUICommandInfo> Grid3x3;

    /** Crosshair */
    TSharedPtr<FUICommandInfo> Crosshair;

    /** Rabatment */
    TSharedPtr<FUICommandInfo> Rabatment;

    /** ActionSafe */
    TSharedPtr<FUICommandInfo> ActionSafe;

    /** TitleSafe */
    TSharedPtr<FUICommandInfo> TitleSafe;

    /** CustomSafe */
    TSharedPtr<FUICommandInfo> CustomSafe;

    /** Letterbox */
    TSharedPtr<FUICommandInfo> Letterbox;

    /**
     * Initialize commands
     */
    virtual void RegisterCommands() override;
};
