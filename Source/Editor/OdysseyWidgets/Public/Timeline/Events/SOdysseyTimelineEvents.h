// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

//Called when zooming occurs
DECLARE_DELEGATE( FOnScrubStarted)
DECLARE_DELEGATE_OneParam( FOnScrubPositionChanged, float) //iOldValue
DECLARE_DELEGATE( FOnScrubStopped )
DECLARE_DELEGATE_OneParam( FOnOffsetChanged, float ) //iOldValue
DECLARE_DELEGATE_OneParam( FOnZoomChanged, float ) //iOldValue
