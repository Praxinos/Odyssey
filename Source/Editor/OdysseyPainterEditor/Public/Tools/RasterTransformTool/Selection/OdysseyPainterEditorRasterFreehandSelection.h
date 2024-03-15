// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "OdysseyPainterEditorRasterFreehandSelection.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterFreehandSelection : public UOdysseyPainterEditorRasterSelection
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterFreehandSelection();

    //Constructor
    UOdysseyPainterEditorRasterFreehandSelection();


public:
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnKeyUp(const FKey& iKey);

    virtual EMouseCursor::Type GetMouseCursor() const override;

private:
    TArray<::ULIS::FRectI> GetSelectionAreaAsScanlines();

};
