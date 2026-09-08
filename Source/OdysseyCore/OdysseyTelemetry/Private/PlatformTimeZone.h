// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CoreMinimal.h"

// It's a custom class as there is no other way to get the timezone via any ue API
// - FPlatformMisc::GetTimeZoneId(); returns an empty string
// - ICU api is not exposed in FInternationalization class
class FPlatformTimeZone
{
public:
    static FString GetTimeZoneId();
};
