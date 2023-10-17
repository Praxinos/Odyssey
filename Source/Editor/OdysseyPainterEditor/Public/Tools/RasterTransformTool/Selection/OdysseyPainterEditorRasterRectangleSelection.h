// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorRasterRectangleSelection.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterRectangleSelection : public UOdysseyPainterEditorRasterSelection
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterRectangleSelection();

    //Constructor
    UOdysseyPainterEditorRasterRectangleSelection();


public:
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseHover(const FOdysseyPoint& iPointInTexture) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey);

private:
    void ConstrainSelectionToRectangle(FVector2D iPosition);

};
