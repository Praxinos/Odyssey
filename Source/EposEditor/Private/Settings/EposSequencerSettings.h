// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "CoreMinimal.h"
#include "SequencerSettings.h"
#include "EposSequencerSettings.generated.h"

//---

UENUM()
enum class EArrangeShots : uint8
{
	/** Origin. */
	OnOneRow UMETA(DisplayName="On a single row"),

	/** Place in Front of Camera. */
	OnTwoRowsShifted UMETA(DisplayName="On 2 rows shifted"),
};

/**
 * Epos Sequencer settings.
 */
UCLASS()
class EPOSEDITOR_API UEposSequencerSettings
	: public USequencerSettings
{
    GENERATED_BODY()

    UEposSequencerSettings();

public:
    
    EArrangeShots GetArrangeShots() const;
    void SetArrangeShots( EArrangeShots iArrangeShots );

protected:

    //virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

	/** Select the way to arrange board sections. */
	UPROPERTY(config, EditAnywhere, Category = "Board")
	EArrangeShots ArrangeShots;
};
