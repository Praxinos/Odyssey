// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputDriver.h"

#include "OdysseyStylusInputSettings.generated.h"

/**
 * Implements the Editor's user settings.
 */
UCLASS(config=EditorPerProjectUserSettings)
class ODYSSEYSTYLUSINPUT_API UOdysseyStylusInputSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    virtual void PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent ) override;
    void RefreshStylusInputDriver();

public:
    /** Driver to use to interpret Stylus inputs. */
    UPROPERTY(config, EditAnywhere, Category=StylusDriver )
    TEnumAsByte<EOdysseyStylusInputDriver> StylusInputDriver;
};
