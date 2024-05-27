// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDPolygon.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorRasterSelection 
{
public:
    FOdysseyPainterEditorRasterSelection(  TArray<FVector2D>& iSelectionArea );
    virtual ~FOdysseyPainterEditorRasterSelection();

public:
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture);
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);

    virtual EMouseCursor::Type GetMouseCursor() const;

    TArray<FVector2D>& GetSelectionArea();
    void SetSelectionArea( TArray<FVector2D>& iSelectionArea );

protected:
    TArray<FVector2D>& mSelectionArea;
};
