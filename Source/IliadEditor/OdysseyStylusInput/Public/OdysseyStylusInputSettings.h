// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"

#include "OdysseyStylusInputSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStylusInputDriverChanged, FName);

/**
 * Implements the StylusInput settings.
 */
UCLASS(config=EditorSettings)
class ODYSSEYSTYLUSINPUT_API UOdysseyStylusInputSettings
    : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    virtual void PostEditChangeProperty( struct FPropertyChangedEvent& iPropertyChangedEvent ) override;

public:
    FName GetStylusDriver() const;
    static FText GetFormatText(FName iStylusInputDriver );

public:
    /** Driver to use to interpret Stylus inputs. */
    UPROPERTY(config, EditAnywhere, Category=StylusDriver )
    FName StylusInputDriver;

    static FOnStylusInputDriverChanged OnStylusInputDriverChanged;
};
