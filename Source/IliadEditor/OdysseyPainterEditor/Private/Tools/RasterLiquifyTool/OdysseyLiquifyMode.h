// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <ULIS>
#include "CoreMinimal.h"
#include "UObject/UObjectGlobals.h"
#include "InputCoreTypes.h"
#include "IPropertyTypeCustomization.h"

#include "OdysseyLiquifyMode.generated.h"

UENUM()
enum class EOdysseyLiquifyMode : uint8
{
    Push,
    Twirl,
    Pinch,
    Expand,
    Crystals,
    Edge,
    Reconstruct,
    Adjust,
    Count UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EOdysseyLiquifyMode, EOdysseyLiquifyMode::Count);

USTRUCT()
struct ODYSSEYPAINTEREDITOR_API FOdysseyLiquifyMode
{
    GENERATED_BODY()

    public:
        static void RegisterDetailCustomization();
        static void UnregisterDetailCustomization();

    public:
        FOdysseyLiquifyMode();

        EOdysseyLiquifyMode Get() const;

    public:
        UPROPERTY(EditAnywhere)
        EOdysseyLiquifyMode Value;
};
