// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "SOdysseyFlipbookTimelineTrackEvents.h"
#include "SOdysseyTimelineEvents.h"

// Called when the selection changes
DECLARE_DELEGATE(FOnPlayStarted)
DECLARE_DELEGATE(FOnPlayStopped)
DECLARE_DELEGATE_OneParam(FOnCurrentKeyframeChanged, int32)
