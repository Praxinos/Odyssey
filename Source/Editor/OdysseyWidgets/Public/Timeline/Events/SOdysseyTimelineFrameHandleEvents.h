// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE( FOnDragStarted)
DECLARE_DELEGATE_OneParam( FOnDragged, int32) //Offset in frames
DECLARE_DELEGATE( FOnDragStopped)

