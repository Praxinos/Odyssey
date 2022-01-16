// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "SOdysseyFlipbookTimelineTrackEvents.h"
#include "SOdysseyTimelineEvents.h"

// Called when the selection changes
DECLARE_DELEGATE(FOnPlayStarted)
DECLARE_DELEGATE(FOnPlayStopped)
DECLARE_DELEGATE_OneParam(FOnCurrentKeyframeChanged, int32)
