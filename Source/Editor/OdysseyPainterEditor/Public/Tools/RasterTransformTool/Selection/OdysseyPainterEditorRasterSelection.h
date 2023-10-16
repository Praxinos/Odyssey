// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorRasterSelection.generated.h"

//This is already a tool to prepare for the moment we'll separate transform and selection. When we'll have a "mask" feature in Odyssey
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterSelection : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterSelection();

    //Constructor
    UOdysseyPainterEditorRasterSelection();

private:


};
