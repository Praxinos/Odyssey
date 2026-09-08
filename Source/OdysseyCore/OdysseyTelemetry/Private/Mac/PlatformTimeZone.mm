// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PlatformTimeZone.h"

#import <Foundation/NSTimeZone.h>

FString
FPlatformTimeZone::GetTimeZoneId()
{
    NSString* Name = [NSTimeZone localTimeZone].name;
    return FString( Name );
}
