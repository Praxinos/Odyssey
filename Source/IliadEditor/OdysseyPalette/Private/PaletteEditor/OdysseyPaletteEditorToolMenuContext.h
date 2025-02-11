// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPaletteEditorToolMenuContext.generated.h"

class FOdysseyPaletteEditorToolkit;

UCLASS()
class UOdysseyPaletteEditorToolMenuContext : public UObject
{
    GENERATED_BODY()

public:
    FOdysseyPaletteEditorToolkit* Toolkit;
};
