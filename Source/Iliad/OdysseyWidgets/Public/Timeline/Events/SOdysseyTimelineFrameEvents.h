// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnGenerateContextMenu, const FGeometry&, const FPointerEvent&)
DECLARE_DELEGATE_TwoParams(FOnDragEnter, const FGeometry&, const FDragDropEvent&)
DECLARE_DELEGATE_OneParam(FOnDragLeave, const FDragDropEvent&)
DECLARE_DELEGATE_RetVal_TwoParams(FReply, FOnDragDetected, const FGeometry&, const FPointerEvent&)

