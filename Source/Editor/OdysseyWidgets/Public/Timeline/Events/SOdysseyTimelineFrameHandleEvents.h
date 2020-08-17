// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE( FOnDragStarted)
DECLARE_DELEGATE_OneParam( FOnDragged, int32) //Offset in frames
DECLARE_DELEGATE( FOnDragStopped)

