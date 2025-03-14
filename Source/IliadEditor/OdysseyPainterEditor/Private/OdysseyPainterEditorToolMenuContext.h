// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPainterEditorToolMenuContext.generated.h"

class FOdysseyPainterEditor;

UCLASS()
class UOdysseyPainterEditorToolMenuContext : public UObject
{
    GENERATED_BODY()

public:
    FOdysseyPainterEditor* PainterEditor;
};
