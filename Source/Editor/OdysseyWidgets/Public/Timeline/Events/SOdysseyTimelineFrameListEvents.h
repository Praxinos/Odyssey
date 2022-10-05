// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "SOdysseyTimelineFrameEvents.h"
#include "SOdysseyTimelineFrameHandleEvents.h"

DECLARE_DELEGATE_RetVal_ThreeParams(FReply, FOnGenerateFrameContextMenu, const FGeometry&, const FPointerEvent&, int32)
DECLARE_DELEGATE_TwoParams( FOnFramesMoved, TArray<int32>, int32)
DECLARE_DELEGATE_OneParam( FOnFramesLengthChanged, TArray<int32>)
DECLARE_DELEGATE( FOnFramesEditStart)
DECLARE_DELEGATE( FOnFramesEditStop)
DECLARE_DELEGATE( FOnFramesEditCancel)

