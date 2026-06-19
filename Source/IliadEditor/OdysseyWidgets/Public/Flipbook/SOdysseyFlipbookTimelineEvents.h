// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

//Called when zooming occurs
DECLARE_DELEGATE( FOnScrubStarted)
DECLARE_DELEGATE_OneParam( FOnScrubPositionChanged, float) //iOldValue
DECLARE_DELEGATE( FOnScrubStopped )
DECLARE_DELEGATE_OneParam( FOnOffsetChanged, float ) //iOldValue
DECLARE_DELEGATE_OneParam( FOnZoomChanged, float ) //iOldValue
