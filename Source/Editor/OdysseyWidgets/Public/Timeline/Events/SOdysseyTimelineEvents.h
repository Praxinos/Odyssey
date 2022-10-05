// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

//Called when zooming occurs
DECLARE_DELEGATE( FOnScrubStarted)
DECLARE_DELEGATE_OneParam( FOnScrubPositionChanged, float) //iOldValue
DECLARE_DELEGATE( FOnScrubStopped )
DECLARE_DELEGATE_OneParam( FOnOffsetChanged, float ) //iOldValue
DECLARE_DELEGATE_OneParam( FOnZoomChanged, float ) //iOldValue
