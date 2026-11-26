// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "OdysseyTiledImageMigrationSettings.generated.h"

/** Remembers whether the user no longer wants the conversion of legacy Odyssey assets offered at startup, per user and project. */
UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Odyssey - Legacy Assets"))
class UOdysseyTiledImageMigrationSettings
    : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    /** Lists these settings under the Plugins category of the editor preferences. */
    UOdysseyTiledImageMigrationSettings();

    /**
     * True when the conversion of the legacy assets is no longer offered at startup.
     * The conversion stays available from the Tools menu and the Odyssey.MigrateToTiledImages console command.
     */
    UPROPERTY(config, EditAnywhere, Category = "Legacy Assets", meta = (DisplayName = "Do Not Offer the Conversion at Startup"))
    bool bDoNotAskAgain = false;
};
